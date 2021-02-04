/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 10:38 BRT
 * Last edited on January 29 of 2021, at 13:06 BRT */

#include <cstring>
#include <experimental/filesystem>
#include <iostream>
#include <random>
#include <sia.hxx>

using namespace std;
using namespace std::experimental::filesystem;

static constexpr char root_name[64] = { '/', 0 };

static vector<string> tokenize(string path) {
    /* We need a function that can tokenize (NOT just split, we need to handle '.' and '..') paths for our file_create
     * function. We pretty much just split the string by '/', and handle '.' and '..' */
    
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

static void gen_id(uint8_t *buf) {
    /* Every .SIA file contains an unique identifier, this function can generate said identifier, it works for any
     * amount of bits (even if we're only gonna call it passing a static value. */
    
    static int count = 0;
    
    if (!count) {
        srand((unsigned)time(0));
        count++;
    }
    
    /* Init the random generator itself. */
    
    static seed_seq seed{ random_device()(), (unsigned)time(0) };
    static default_random_engine rng(seed);
    static uniform_int_distribution<uint8_t> dist(0, 15);
    
    /* And generate the random number(s). */
    
    for (uint64_t i = 0; i < 16; i++) {
        buf[i] = dist(rng);
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
    /* Create the file entry struct, set the filename, flags, and write it into the offset that alloc_file_entry (or
     * whatever other function) specified. */
    
    sia_file_t fs = { 0 };
    char nam[64] = { 0 };
    
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
    /* Create the data struct, set the data (duh), and write it into the offset that alloc_data_entry (or whatever
     * other function) specified. */
    
    sia_data_t data = { 0 };
    
    return write_bytes(file, &data, sizeof(sia_data_t), off);
}

static uint64_t alloc_data_entry(fstream &file) {
    /* Call alloc_data_entry_int, passing the end of the file as the offset. */
    
    file.seekg(0, fstream::end);
    uint64_t off = file.tellg();
    
    return alloc_data_entry_int(file, off) ? off : 0;
}

static bool file_write(fstream &hfile, sia_file_t &file, uint64_t foff, const void *buf, uint64_t size) {
    sia_data_t data;
    uint64_t ret = 0;
    uint64_t last = file.offset;
    
    /* First write to this file? So create the initial data struct. */
    
    if (last == 0) {
        if (!(file.offset = last = alloc_data_entry(hfile))) {
            return false;
        }
    }
    
    /* Let's write the contents of the buffer into the SIA file. */
    
    while (size) {		
        /* Read the data struct. */
        
        if (!read_bytes(hfile, &data, sizeof(sia_data_t), last)) {
            return false;
        }
        
        /* Copy the contents of the current "sector" of the buffer into the current data buffer. */
        
        uint64_t sz = sizeof(sia_data_t::data);
        
        if (sz > size) {
            sz = size;
        }
        
        memcpy(data.data, static_cast<const char*>(buf) + ret, sz);
        
        /* And write it back into the SIA file. */
        
        if (!write_bytes(hfile, &data, sizeof(sia_data_t), last)) {
            return false;
        }
        
        ret += sz;
        size -= sz;
        
        /* If the next entry isn't allocated, and we still have data to copy, allocate a new entry. */
        
        if (!data.next && size) {
            if (!(data.next = alloc_data_entry(hfile))) {
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

static bool get_image(fstream &hfile, sia_header_t &header, uint8_t num, sia_file_t *outf, uint64_t *outo) {
    /* The image can have multiple root directories, and they are all stored at the root_images field, let's check if
     * the image num is inside the valid range, and if the offset isn't zero (after that, it's just reading the struct
     * and saving the offset). */

    if (num >= sizeof(sia_header_t::root_images) / sizeof(uint64_t)) {
        return false;
    } else if (!header.root_images[num]) {
        return false;
    } else if (!read_bytes(hfile, outf, sizeof(sia_file_t), header.root_images[num])) {
        return false;
    }

    *outo = header.root_images[num];

    return true;
}

static bool file_find(fstream &hfile, sia_file_t &dir, string name, sia_file_t *outf, uint64_t *outo) {
    /* If the dir offset is 0, this directory doesn't have any files inside of it. */
    
    if (dir.offset == 0) {
        return false;
    }
    
    /* If it isn't 0, it's the offset for the first file inside of this directory, so let's read it, and start
     * searching! */
    
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

static bool link_file_entry(fstream &hfile, sia_file_t dir, sia_file_t file, uint64_t last, uint64_t n) {
	/* First entry (offset = 0): Just set the offset field of the directory, write the file struct, and we're done.
	 * Other entries: We need to find where this entry goes alphabetically. */
	
	if (dir.offset == 0) {
		dir.offset = n;
		return write_bytes(hfile, &dir, sizeof(sia_file_t), last);
	} else {
		last = dir.offset;
		
		if (!read_bytes(hfile, &dir, sizeof(sia_file_t), last)) {
			return false;
		}
	}

	string name(file.name);
	
	while (dir.next != 0) {
		last = dir.next;
		
		if (!read_bytes(hfile, &dir, sizeof(sia_file_t), last)) {
			return false;
		} else if (name > dir.name) {
		    break;
		}
	}

	/* If we're the last entry now, we can just set the .next field of the dir entry before us, else, we need to set
	 * our next field to the old .next value as well. */

    if (dir.next != 0) {
        file.next = dir.next;

        if (!write_bytes(hfile, &file, sizeof(sia_file_t), n)) {
            return false;
        }
    }
	
	dir.next = n;
	
	return write_bytes(hfile, &dir, sizeof(sia_file_t), last);
}

static bool file_create(fstream &hfile, sia_header_t &header, uint8_t num, string spath, string dpath,
                        uint16_t flags) {
	sia_file_t file;
	sia_file_t dir;
	uint64_t doff;
	
	if (!get_image(hfile, header, num, &dir, &doff)) {
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
		
		if (res && !(file.flags & SIA_DIR)) {
			/* Yes, and it's NOT a directory... */
			cout << "Error: '" << toks.front() << "' is not a directory." << endl;
			return false;
		} else if (!res) {
			/* Alloc the file entry, link it to the parent directory, and read it back into a sia_file_t struct. */

			sia_file_t fst;
			uint64_t fs = alloc_file_entry(hfile, toks.front(), flags | SIA_DIR);
			
			if (fs == 0) {
				cout << "Error: Couldn't create one of the parent directories of '" << dpath << "'." << endl;
				return false;
            } else if (!read_bytes(hfile, &fst, sizeof(sia_file_t), fs)) {
				cout << "Error: Couldn't create one of the parent directories of '" << dpath << "'." << endl;
				return false;
			} else if (!link_file_entry(hfile, dir, fst, doff, fs)) {
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
	
	/* Now, create the file itself, and just like the directories that we (probably) had to create to get here, the
	 * process is: alloc the file entry, link it to the parent directory, and read it back into the sia_file_t
	 * struct. */

	sia_file_t fst;
	uint64_t fs = alloc_file_entry(hfile, toks.front(), flags);
	
	if (fs == 0) {
		cout << "Error: Couldn't create '" << dpath << "'." << endl;
		return false;
    } else if (!read_bytes(hfile, &fst, sizeof(sia_file_t), fs)) {
		cout << "Error: Couldn't create '" << dpath << "'." << endl;
		return false;
	} else if (!link_file_entry(hfile, dir, fst, doff, fs)) {
		cout << "Error: Couldn't create '" << dpath << "'." << endl;
		return false;
	} else if (!read_bytes(hfile, &dir, sizeof(sia_file_t), doff)) {
		cout << "Error: Couldn't create '" << dpath << "'." << endl;
		return false;
	} else if (!file_find(hfile, dir, toks.front(), &file, &doff)) {
		cout << "Error: Couldn't create '" << dpath << "'." << endl;
		return false;
	}
	
	/* Time to write the contents of the file 'spath' into the file that we just created. Start by checking if we
	 * really need to do this, and, by reading the source file. */
	
	if (spath == "") {
		return true;
	}
	
	fstream stream(spath);
	
	if (!stream.is_open()) {
		cout << "Error: Couldn't open the file '" << spath << "'." << endl;
		return false;
	}
	
	/* Now we need to read the file into memory, seek to the end of the file, get the position (that's the file size),
	 * alloc space for reading everything, and call the read function. */
	
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

bool sia_add_image(sia_t &sia, string base) {
    /* This function takes a path (the base path) + the sia state struct and creates a new root image (if there's
     * space for doing it). First, we need to check if the last_root_image is smaller than the amount of root images
     * that we can have (remembering that it starts at -1). */

    if (sia.last_root_image + 1 >= sizeof(sia_header_t::root_images) / sizeof(uint64_t)) {
        cout << "Error: All the SIA root images are already full." << endl;
        return false;
    }

    /* Now we can just initialize the root directory structure, write the offset into the right index of root_images,
     * write the structures into the memory, and go into creating all the files. */

    sia_file_t root = { 0, SIA_DIR | SIA_READ | SIA_WRITE | SIA_EXEC, 0, 0, 0 };
    uint8_t num = ++sia.last_root_image;

    memcpy(root.name, root_name, 64);

    /* Getting where we should put this root directory... */

    sia.file.seekg(0, fstream::end);
    uint64_t off = sia.file.tellg();

    sia.header.root_images[num] = off;

    if (!write_bytes(sia.file, &sia.header, sizeof(sia_header_t), 0)) {
        cout << "Error: Couldn't create one of the SIA root images." << endl;
        return false;
    } else if (!write_bytes(sia.file, &root, sizeof(sia_file_t), off)) {
        cout << "Error: Couldn't create one of the SIA root images." << endl;
        return false;
    }

    /* Now, we can create all the files. */

    try {
		for (auto &p : recursive_directory_iterator(base)) {
			/* Get both the source file full path, and the destination file path. */
			
			bool res;
			perms sperms = status(p.path()).permissions();
			string sfile = p.path().string();
			string dfile = sfile.substr(base.size());
			
			/* file_create can handle both directories and files, but each one requires some different arguments... */
			
			uint16_t dperms = 0;
			
			if ((sperms & perms::owner_read) == perms::owner_read) {
				dperms |= SIA_READ;
			}
			
			if ((sperms & perms::owner_write) == perms::owner_write) {
				dperms |= SIA_WRITE;
			}
			
			if ((sperms & perms::owner_exec) == perms::owner_exec) {
				dperms |= SIA_EXEC;
			}
			
			if (is_directory(p)) {
				res = file_create(sia.file, sia.header, num, "", dfile, SIA_DIR | dperms);
			} else {
				res = file_create(sia.file, sia.header, num, sfile, dfile, dperms);
			}
			
			if (!res) {
				return false;
			}
		}
	} catch (filesystem_error &err) {
		cout << "Error: The base folder (" << base << ") doesn't exists or is a file." << endl;
		return false;
	}

	return true;
}

bool sia_add_kernel(sia_t &sia, std::string base, uint16_t flags) {
    /* The initial checks are simillar to the ones in sia_add_image, but we use the last_kernel_image field. */

    if (sia.last_kernel_image + 1 >= sizeof(sia_header_t::kernel_images) / sizeof(uint64_t)) {
        cout << "Error: All the SIA kernel images are already full." << endl;
        return false;
    }

    fstream stream(base);

    if (!stream.is_open()) {
        cout << "Error: Couldn't open the kernel file (" << base << ")." << endl;
        return false;
    }

    /* Get the file size and read its contents into memory. */

    stream.seekg(0, fstream::end);
    uint64_t size = stream.tellg();
    stream.seekg(0);

    unique_ptr<char[]> buf = make_unique<char[]>(size);
    stream.read(&buf[0], size);

    if (stream.fail()) {
        cout << "Error: Couldn't read the kernel file (" << base << ")." << endl;
        stream.close();
        return false;
    }

    stream.close();

    /* Now we need to create the file entry and write the kernel contents into that file. */

    sia.file.seekg(0, fstream::end);

    uint64_t off;
    sia_file_t file;
    uint8_t num = ++sia.last_kernel_image;

    if (!(off = sia.header.kernel_images[num] = alloc_file_entry(sia.file, "oskrnl.elf", flags))) {
        cout << "Error: Couldn't write to the kernel file entry." << endl;
        return false;
    } else if (!write_bytes(sia.file, &sia.header, sizeof(sia_header_t), 0)) {
        cout << "Error: Couldn't write to the kernel file entry." << endl;
        return false;
    } else if (!read_bytes(sia.file, &file, sizeof(sia_file_t), off)) {
        cout << "Error: Couldn't write to the kernel file entry." << endl;
        return false;
    } else if (!file_write(sia.file, file, off, &buf[0], size)) {
        cout << "Error: Couldn't write to the kernel file entry." << endl;
        return false;
    }

    return true;
}

bool sia_init(sia_t &sia) {
    /* This function properly initializes the sia state struct and writes the sia header (in it's pre kernel/root
     * images state) into the file stream (which is the only thing we expect to be already initialized, though we
     * do expect .header to already be pointed into the header temp buffer). */

    memset(&sia.header, 0, sizeof(sia_header_t));
    gen_id(sia.header.id);

    sia.last_kernel_image = sia.last_root_image = -1;
    sia.header.magic = SIA_MAGIC;
    sia.header.info = SIA_FIXED;

    if (!write_bytes(sia.file, &sia.header, sizeof(sia_header_t), 0)) {
        cout << "Error: Couldn't create the SIA header." << endl;
        return false;
    }

    return true;
}
