# ChecksumAndCopy
check the MD5 of file and copy it to U-disk

Use IDE is VC6.0 (C++ and MFC).

当前的识别规则，所有的MD5写在txt文件里，格式如下：
804aaa4bb5f7779e5adee8e7ef2266bc qwer.bin
即不需包含MD5值，和包含文件名，中间用空格分隔。
和在Linux下使用md5sum命令产生的结果一致。

工作原理：会读取每一个的txt文件里的每一行，识别MD5和文件名。
