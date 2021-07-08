/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 29 of 2021, at 10:13 BRT
 * Last edited on July 08 of 2021, at 08:51 BRT */

#include <cstring>
#include <iostream>
#include <sia.hxx>
#include <vector>

using namespace std;

int main(int argc, char **argv) {
    vector<string> roots, kernels;
    string dest;

    /* First, parse the given arguments, they contain information (or should contain) about the destination path, the
     * root images and kernel images (or if the user is just requesting help). */

    if (argc == 1) {
        cout << "CHicago SIA (System Image Archive) creation tool" << endl;
        cout << "Usage: " << argv[0] << " [options]" << endl;
        return 0;
    }

    for (int i = 1; i < argc; i++) {
        if (!strcmp(argv[i], "-v") || !strcmp(argv[i], "--version")) {
            cout << "CHicago SIA (System Image Archive) creation tool" << endl;
            cout << "Version 1.2" << endl;
            return 0;
        } else if (!strcmp(argv[i], "-h") || !strcmp(argv[i], "--help")) {
            cout << "Usage: " << argv[0] << " [options]" << endl << endl;
            cout << "Valid options:" << endl;
            cout << "     -v | --version        Print the version of this program." << endl;
            cout << "     -h | --help           Print this help text." << endl;
            cout << "     -o | --output         Set the output file." << endl;
            cout << "     -i | --image          Add a new root image (max amount of root images is "
                 << sizeof(sia_header_t::root_images) / sizeof(uint64_t) << ")." << endl;
            cout << "     -k | --kernel         Add a new kernel image (max amount of kernel images is "
                 << sizeof(sia_header_t::kernel_images) / sizeof(uint64_t) << ")." << endl << endl;
            cout << "The -k|--kernel argument is on the format of <flags>:<path>:<symfile>."
                 << "Said flags are architecture specific. Here is a little list of kernel arch flags:" << endl;
            cout << "    x86: 0x01 (SIA_X86, indicates that this is a x86 image)." << endl
                 << "    amd64: 0x02 (SIA_AMD64, indicates that this is an amd64 image)." << endl;
            return 0;
        } else if (!strcmp(argv[i], "-o") || !strcmp(argv[i], "--output")) {
            if (i + 1 >= argc) {
                cout << "Error: Expected the output file name after -o/--output." << endl;
                return 1;
            }

            dest = argv[++i];
        } else if (!strcmp(argv[i], "-i") || !strcmp(argv[i], "--image")) {
            if (i + 1 >= argc) {
                cout << "Error: Expected the source path after -i/--image." << endl;
                return 1;
            }

            roots.push_back(argv[++i]);
        } else if (!strcmp(argv[i], "-k") || !strcmp(argv[i], "--kernel")) {
            if (i + 1 >= argc) {
                cout << "Error: Expected the kernel source file after -k/--kernel." << endl;
                return 1;
            }

            kernels.push_back(argv[++i]);
        } else {
            cout << "Error: Invalid argument '" << argv[i] << "'";
            return 1;
        }
    }

    if (dest.empty()) {
        cout << "Error: Expected the output file name." << endl;
        return 1;
    } else if (roots.empty()) {
        cout << "Error: Expected at least one root image." << endl;
        return 1;
    }

    /* Create the fstream for the output file, the sia_header_t temp buffer, our sia_t state struct, and call sia_init
     * to prepare everything. */

    sia_header_t header;
    fstream file(dest, fstream::in | fstream::out | fstream::binary | fstream::trunc);

    if (file.fail()) {
        cout << "Error: Couldn't create the SIA file (" << dest << ")." << endl;
        return 1;
    }

    sia_t sia = { file, header, 0, 0 };

    if (!sia_init(sia)) {
        file.close();
        return 1;
    }

    /* Now let's create all the root images. */

    for (string root : roots) {
        if (!sia_add_image(sia, root)) {
            file.close();
            return 1;
        }
    }

    /* And all the kernel images, but those are more complex, as we need to parse the flags, and make sure that the
     * user also passed the kernel symbol file. */

    for (string kernel : kernels) {
        if (!(kernel[0] >= '0' && kernel[0] <= '9')) {
            cout <<  "Error: Expected the kernel flags before the kernel file name." << endl;
            return 1;
        }

        size_t pos;
        uint64_t flags = (uint16_t)stoul(kernel, &pos, 0);

        if (kernel[pos] != ':') {
            cout << "Error: Expected a colon after the kernel flags." << endl;
            return 1;
        }

        string base = kernel.substr(pos + 1);

        pos = base.find(":");

        if (pos == string::npos) {
            cout << "Error: Expected a colon after the kernel file name." << endl;
            return 1;
        } else if (!sia_add_kernel(sia, base.substr(0, pos), base.substr(pos + 1), flags)) {
            file.close();
            return 1;
        }
    }

    file.close();
    
    return 0;
}
