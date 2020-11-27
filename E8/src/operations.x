struct file_in{
    string fileName <50>;
};

struct dir_in{
    string dirName <50>;
    int num;
};

struct file_out{
    int chars;
    int strings;
    int rows;
};

program OPERATIONSPROG {
	version OPERATIONSVERS {
		file_out FILE_SCAN(file_in) = 1;
		int DIR_SCAN(dir_in) = 2;
	} = 1;
} = 0x20000013;