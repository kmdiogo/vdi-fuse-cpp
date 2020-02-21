#include <iostream>
#include "include/Ext2.h"

using namespace std;

int main() {

    const string testFile = "/mnt/e/UbuntuShared/test.vdi";
    VDI vdi = VDI();
    vdi.open(testFile);

    Ext2 ext = Ext2(&vdi);

    Directory dir = ext.fetchDirectory(2);
    dir.open();
    ext.traverse(&dir);
    dir.close();

    vdi.close();
    return 0;
}
