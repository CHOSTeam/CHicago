/* File author is Ítalo Lima Marconato Matias
 *
 * Created on January 22 of 2021, at 17:39 BRT
 * Last edited on July 04 of 2021 at 11:13 BRT */

#include <efi/lib.h>
#include <efi/loaded_image.h>

EfiStatus EfiOpenFile(Char16 *Path, UInt8 Mode, EfiFile **Out) {
    if (Path == Null || Out == Null) return EFI_INVALID_PARAMETER;

    /* Open the root/ESP volume, and redirect to ->Open (while passing some default values). */

    EfiFile *root;
    EfiLoadedImage *li;
    EfiSimpleFileSystem *vol;
    EfiStatus status = EfiBS->HandleProtocol(EfiIH, &EfiLoadedImageGuid, (Void**)&li);

    if (EFI_ERROR(status) ||
        EFI_ERROR((status = EfiBS->HandleProtocol(li->DeviceHandle, &EfiSimpleFileSystemGuid, (Void**)&vol))) ||
        EFI_ERROR((status = vol->OpenVolume(vol, &root)))) return status;

    return root->Open(root, Out, Path, Mode, 0);
}

UIntN EfiGetFileSize(EfiFile *File) {
    /* Block invalid arguments, and call GetInfo (two times, first to get the size of the buffer that we need, and the)
     * second to actually get the file info. */

    if (File == Null) return 0;

    UIntN size = 0;
    EfiFileInfo *info = Null;
    EfiStatus status = File->GetInfo(File, &EfiFileInfoGuid, &size, info);

    if (status != EFI_BUFFER_TOO_SMALL || (info = EfiAllocatePool(size)) == Null) return 0;
    else if (EFI_ERROR((status = File->GetInfo(File, &EfiFileInfoGuid, &size, info)))) {
        EfiFreePool(info);
        return 0;
    }

    size = info->FileSize;

    EfiFreePool(info);

    return size;
}
