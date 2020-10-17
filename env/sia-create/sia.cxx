/* File author is Ítalo Lima Marconato Matias
 *
 * Created on June 20 of 2020, at 16:00 BRT
 * Last edited on October 10 of 2020, at 15:43 BRT */

#include <cstring>
#include <ctime>
#include <experimental/filesystem>
#include <fstream>
#include <iostream>
#include <memory>
#include <random>
#include <sia.hxx>

using namespace std::experimental::filesystem;

static constexpr char rootName[64] = { '/', 0 };

static vector<string> tokenize(string path) {
	/* We need a function that can tokenize (NOT just split, we need to handle '.' and '..')
	 * paths for our file_create function. We pretty much just split the string by '/', and
     * handle '.' and '..' */
	
	vector<string> ret;
	stringstream p(path);
	string tok;
	
	while (getline(p, tok, '/')) {
		if (!tok.compare(".") || tok.empty() ||
			(!tok.compare("..") && ret.size() < 1)) {
			continue;
		} else if (!tok.compare("..")) {
			ret.pop_back();
		} else {
			ret.push_back(tok);
		}
	}
	
	return ret;
}

static void gen_rand(uint8_t *buf, uint64_t bits) {
	/* Every .SIA file contains a UUID, this function can generate said UUID, it works
	 * for any amount of bits (even if we're only gonna call it passing a static value. */
	
	static int count = 0;
	
	if (count == 0) {
		srand((unsigned)time(0));
		count++;
	}
	
	/* Init the random generator itself. */
	
	static seed_seq seed{ random_device()(), (unsigned)time(0) };
	static default_random_engine rng(seed);
	static uniform_int_distribution<uint8_t> dist(0, 15);
	
	/* And generate the random number(s). */
	
	for (uint64_t i = 0; i < bits / 8; i++) {
		buf[bits / 8 - i - 1] = dist(rng) << 8 | dist(rng);
	}
}

/* Helper functions for reading/writing into/from any point of a file. */

static bool read_bytes(fstream &file, void *data, uint64_t size, uint64_t off) {
	file.seekg(off);
	
	if (file.fail()) {
		return false;
	}
	
	file.read(reinterpret_cast<char*>(data), size);
	
	return !file.fail();
}

static bool write_bytes(fstream &file, void *data, uint64_t size, uint64_t off) {
	file.seekg(off);
	
	if (file.fail()) {
		return false;
	}
	
	file.write(reinterpret_cast<char*>(data), size);
	
	return !file.fail();
}

static bool alloc_file_entry_int(fstream &file, string name, uint16_t flags, uint64_t off) {
	/* Create the file entry struct, set the filename, flags, and write it into the offset
	 * that alloc_file_entry (or whatever other function) specified. */
	
	sia_file_t fs;
	char nam[64] = { 0 };
	
	memset(&fs, 0, sizeof(sia_file_t));
	memcpy(nam, &name[0], name.size() > 63 ? 63 : name.size());
	memcpy(fs.name, nam, 64);
	
	fs.flags = flags;
	
	return write_bytes(file, &fs, sizeof(sia_file_t), off);
}

static uint64_t alloc_file_entry(fstream &file, string name, uint16_t flags) {
	/* Call alloc_file_entry_int, passing the end of the file as the offset. */
	
	file.seekg(0, fstream::end);
	uint64_t off = file.tellg();
	
	return alloc_file_entry_int(file, name, flags, off) ? off : 0;
}

static bool alloc_data_entry_int(fstream &file, uint64_t off) {
	/* Create the data struct, set the data (duh), and write it into the offset
	 * that alloc_data_entry (or whatever other function) specified. */
	
	sia_data_t data;
	memset(&data, 0, sizeof(sia_data_t));
	
	return write_bytes(file, &data, sizeof(sia_data_t), off);
}

static uint64_t alloc_data_entry(fstream &file) {
	/* Call alloc_data_entry_int, passing the end of the file as the offset. */
	
	file.seekg(0, fstream::end);
	uint64_t off = file.tellg();
	
	return alloc_data_entry_int(file, off) ? off : 0;
}

bool file_write(fstream &hfile, sia_file_t &file, uint64_t foff, const void *buf, uint64_t size) {
	sia_data_t data;
	uint64_t ret = 0;
	uint64_t last = file.offset;
	
	/* First write to this file? So create the initial data struct. */
	
	if (last == 0) {
		if ((file.offset = last = alloc_data_entry(hfile)) == 0) {
			return false;
		}
	}
	
	/* Let's write the contents of the buffer into the SIA file. */
	
	while (size) {		
		/* Read the data struct. */
		
		if (!read_bytes(hfile, &data, sizeof(sia_data_t), last)) {
			return false;
		}
		
		/* Copy the contents of the current "sector" of the buffer into data.contents. */
		
		uint64_t sz = sizeof(data.contents);
		
		if (sz > size) {
			sz = size;
		}
		
		memcpy(data.contents, static_cast<const char*>(buf) + ret, sz);
		
		/* And write it back into the SIA file. */
		
		if (!write_bytes(hfile, &data, sizeof(sia_data_t), last)) {
			return false;
		}
		
		ret += sz;
		size -= sz;
		
		/* If the next entry isn't allocated, and we still have data to copy, allocate a new entry. */
		
		if (data.next == 0 && size) {
			if ((data.next = alloc_data_entry(hfile)) == 0) {
				return false;
			} else if (!write_bytes(hfile, &data, sizeof(sia_data_t), last)) {
				return false;
			}
		}
		
		last = data.next;
	}
	
	/* Set the file size in the struct, and write the struct back into the SIA file. */
	
	file.size = ret > file.size ? ret : file.size;
	
	return write_bytes(hfile, &file, sizeof(sia_file_t), foff);
}

bool get_root(fstream &hfile, sia_header_t &header, sia_file_t *outf, uint64_t *outo) {
	/* The root directory is stored at the root_offset field of the header.
	   Just read the file struct at that offset lol. */
	
	if (!read_bytes(hfile, outf, sizeof(sia_file_t), header.root_offset)) {
		return false;
	}
	
	*outo = header.root_offset;
	
	return true;
}

bool file_find(fstream &hfile, sia_file_t &dir, string name, sia_file_t *outf, uint64_t *outo) {
	/* If the dir offset is 0, this directory doesn't have any files inside of it. */
	
	if (dir.offset == 0) {
		return false;
	}
	
	/* If it isn't 0, it's the offset for the first file inside of this directory, so let's
	 * read it, and start searching! */
	
	uint64_t last = dir.offset;
	sia_file_t cur;
	
	if (!read_bytes(hfile, &cur, sizeof(sia_file_t), last)) {
		return false;
	}
	
	while (true) {
		if (!string(cur.name).compare(name)) {
			/* Name match, copy the file struct and the offset, and return. */
			
			memcpy(outf, &cur, sizeof(sia_file_t));
			*outo = last;
			
			return true;
		}
		
		/* Save the next offset, and read it (unless it's 0, in that case, we're done. */
		
		last = cur.next;
		
		if (last == 0) {
			break;
		} else if (!read_bytes(hfile, &cur, sizeof(sia_file_t), last)) {
			return false;
		}
	}
	
	return false;
}

static bool link_file_entry(fstream &hfile, sia_file_t dir, uint64_t last, uint64_t n) {
	/* First entry (offset = 0): Just set the offset field of the directory, write the file struct,
	 *						 and we're done.
	 * Other entries: We need to find the very last entry, and write the file offset (n) to that last
	 *				  entry offset field (which is going to be called .next, instead of .offset). */
	
	if (dir.offset == 0) {
		dir.offset = n;
		return write_bytes(hfile, &dir, sizeof(sia_file_t), last);
	} else {
		last = dir.offset;
		
		if (!read_bytes(hfile, &dir, sizeof(sia_file_t), last)) {
			return false;
		}
	}
	
	while (dir.next != 0) {
		last = dir.next;
		
		if (!read_bytes(hfile, &dir, sizeof(sia_file_t), last)) {
			return false;
		}
	}
	
	dir.next = n;
	
	return write_bytes(hfile, &dir, sizeof(sia_file_t), last);
}

bool file_create(fstream &hfile, sia_header_t &header, string spath, string dpath, uint16_t flags) {
	sia_file_t file;
	sia_file_t dir;
	uint64_t doff;
	
	if (!get_root(hfile, header, &dir, &doff)) {
		/* I don't know how at this point this would fail, but, we need to handle that possibility. */
		
		cout << "Error: Failed to open the root directory." << endl;
		return false;
	}
	
	/* Tokenize the destination path. */
	
	vector<string> toks = tokenize(dpath);
	
	if (toks.size() == 0) {
		cout << "Error: Destination path is empty." << endl;
		return false;
	}
	
	/* Now, find the parent directory of the file that we're going to create. */
	
	while (toks.size() != 1) {
		/* We're going to recursively create the directories that we need, does this one exists? */
		
		bool res = file_find(hfile, dir, toks.front(), &file, &doff);
		
		if (res && !(file.flags & SIA_FLAGS_DIRECTORY)) {
			/* Yes, and it's NOT a directory... */
			
			cout << "Error: '" << toks.front() << "' is not a directory." << endl;
			return false;
		} else if (!res) {
			/* Alloc the file entry, link it to the parent directory, and read it back into a
			 * sia_file_t struct. */
			
			uint64_t fs = alloc_file_entry(hfile, toks.front(), flags | SIA_FLAGS_DIRECTORY);
			
			if (fs == 0) {
				cout << "Error: Couldn't create one of the parent directories of '" << dpath << "'." << endl;
				return false;
			} else if (!link_file_entry(hfile, dir, doff, fs)) {
				cout << "Error: Couldn't create one of the parent directories of '" << dpath << "'." << endl;
				return false;
			} else if (!read_bytes(hfile, &dir, sizeof(sia_file_t), doff)) {
				cout << "Error: Couldn't create one of the parent directories of '" << dpath << "'." << endl;
				return false;
			} else if (!file_find(hfile, dir, toks.front(), &file, &doff)) {
				cout << "Error: Couldn't create one of the parent directories of '" << dpath << "'." << endl;
				return false;
			}
		}
		
		memcpy(&dir, &file, sizeof(sia_file_t));
		toks.erase(toks.begin());
	}
	
	/* Now, create the file itself, and just like the directories that we (probably) had to create
	 * to get here, the process is: alloc the file entry, link it to the parent directory, and read
	 * it back into the sia_file_t struct. */
	
	uint64_t fs = alloc_file_entry(hfile, toks.front(), flags);
	
	if (fs == 0) {
		cout << "Error: Couldn't create '" << dpath << "'." << endl;
		return false;
	} else if (!link_file_entry(hfile, dir, doff, fs)) {
		cout << "Error: Couldn't create '" << dpath << "'." << endl;
		return false;
	} else if (!read_bytes(hfile, &dir, sizeof(sia_file_t), doff)) {
		cout << "Error: Couldn't create '" << dpath << "'." << endl;
		return false;
	} else if (!file_find(hfile, dir, toks.front(), &file, &doff)) {
		cout << "Error: Couldn't create '" << dpath << "'." << endl;
		return false;
	}
	
	/* Time to write the contents of the file 'spath' into the file that we just created.
	 * Start by checking if we really need to do this, and, by reading the source file. */
	
	if (spath == "") {
		return true;
	}
	
	fstream stream(spath);
	
	if (!stream.is_open()) {
		cout << "Error: Couldn't open the file '" << spath << "'." << endl;
		return false;
	}
	
	/* Now we need to read the kernel file into memory, seek to the end of the file,
	 * get the position (that's the file size), alloc space for reading everything,
	 * and call the read function. */
	
	stream.seekg(0, fstream::end);
	uint64_t size = stream.tellg();
	stream.seekg(0);
	
	unique_ptr<char[]> buf = make_unique<char[]>(size);
	
	stream.read(&buf[0], size);
	
	if (stream.fail()) {
		cout << "Error: Couldn't read the file (" << spath << ")." << endl;
		stream.close();
		return false;
	}
	
	stream.close();
	
	/* Finally, write the file contents! */
	
	if (!file_write(hfile, file, doff, &buf[0], size)) {
		cout << "Error: Couldn't write to the file '" << dpath << "'." << endl;
		return false;
	}
	
	return true;
}

bool sia_create(string path, string kernel, string base) {
	/* This is the main function of this program, it takes a path (destination path), a kernel
	 * path, and a base path (which will be copied as the initrd), and create a SIA file which
	 * can be used by the bootloader.
	 * Let's create the destination file, and, if it already exists, truncate it to zero bytes. */
	
	fstream hfile(path, fstream::in | fstream::out | fstream::binary | fstream::trunc);
	
	if (hfile.fail()) {
		cout << "Error: Couldn't create the SIA file (" << path << ")." << endl;
		return false;
	}
	
	/* Now, let's create the header. */
	
	sia_header_t header;
	sia_file_t root;
	
	memset(&header, 0, sizeof(sia_header_t));
	memset(&root, 0, sizeof(sia_file_t));
	gen_rand(header.uuid, 128);
	
	header.magic = SIA_MAGIC;
	header.root_offset = sizeof(sia_header_t);
	header.info = SIA_INFO_KERNEL | SIA_INFO_FIXED;
	root.flags = SIA_FLAGS_DIRECTORY |
				 SIA_FLAGS_READ | SIA_FLAGS_WRITE | SIA_FLAGS_EXEC;
	memcpy(root.name, rootName, 64);
	
	/* Write both the header and the root directory to the file. */
	
	if (!write_bytes(hfile, &header, sizeof(sia_header_t), 0)) {
		cout << "Error: Couldn't create the SIA header." << endl;
		hfile.close();
		return false;
	} else if (!write_bytes(hfile, &root, sizeof(sia_file_t), header.root_offset)) {
		cout << "Error: Couldn't create the SIA header." << endl;
		hfile.close();
		return false;
	}
	
	/* Time to set the kernel image, first, let's try to open it. */
	
	fstream stream(kernel);
	sia_file_t file;
	
	if (!stream.is_open()) {
		cout << "Error: Couldn't open the kernel file (" << kernel << ")." << endl;
		hfile.close();
		return false;
	}
	
	/* Now we need to read the kernel file into memory, seek to the end of the file,
	 * get the position (that's the file size), alloc space for reading everything,
	 * and call the read function. */
	
	stream.seekg(0, fstream::end);
	uint64_t size = stream.tellg();
	stream.seekg(0);
	
	unique_ptr<char[]> buf = make_unique<char[]>(size);
	
	stream.read(&buf[0], size);
	
	/* Now we need to create the file entry for the kernel, copy the kernel contents,
	 * update the header, and write the file (not the header, the header we're only going
	 * to write in the end. */
	
	if (stream.fail()) {
		cout << "Error: Couldn't read the kernel file (" << kernel << ")." << endl;
		stream.close();
		hfile.close();
		return false;
	}
	
	stream.close();
	
	if ((header.kernel_offset = alloc_file_entry(hfile, "chkrnl.elf", SIA_FLAGS_READ)) == 0) {
		cout << "Error: Couldn't write to the kernel file entry." << endl;
		hfile.close();
		return false;
	} else if (!read_bytes(hfile, &file, sizeof(sia_file_t), header.kernel_offset)) {
		cout << "Error: Couldn't write to the kernel file entry." << endl;
		hfile.close();
		return false;
	} else if (!file_write(hfile, file, header.kernel_offset, &buf[0], size)) {
		cout << "Error: Couldn't write to the kernel file entry." << endl;
		hfile.close();
		return false;
	}
	
	/* Finally, it's time to write everything inside of the base folder. */
	
	try {
		for (auto &p : recursive_directory_iterator(base)) {
			/* Get both the source file full path, and the destination file
			 * path. */
			
			bool res;
			perms sperms = status(p.path()).permissions();
			string sfile = p.path().string();
			string dfile = sfile.substr(base.size());
			
			/* file_create can handle both directories and files, but each one
			 * requires some different arguments... */
			
			uint16_t dperms = 0;
			
			if ((sperms & perms::owner_read) == perms::owner_read) {
				dperms |= SIA_FLAGS_READ;
			}
			
			if ((sperms & perms::owner_write) == perms::owner_write) {
				dperms |= SIA_FLAGS_WRITE;
			}
			
			if ((sperms & perms::owner_exec) == perms::owner_exec) {
				dperms |= SIA_FLAGS_EXEC;
			}
			
			if (is_directory(p)) {
				res = file_create(hfile, header, "", dfile,
								  SIA_FLAGS_DIRECTORY | dperms);
			} else {
				res = file_create(hfile, header, sfile, dfile, dperms);
			}
			
			if (!res) {
				hfile.close();
				return false;
			}
		}
	} catch (filesystem_error &err) {
		cout << "Error: The base folder (" << base << ") doesn't exists or is a file." << endl;
		hfile.close();
		return false;
	}
	
	/* Write the header, and we're finished :) */
	
	if (!write_bytes(hfile, &header, sizeof(sia_header_t), 0)) {
		cout << "Error: Couldn't write the SIA header." << endl;
		hfile.close();
		return false;
	}
	
	hfile.close();
	
	return true;
}
