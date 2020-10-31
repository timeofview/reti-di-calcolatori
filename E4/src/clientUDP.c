#include <string.h>
#include <fcntl.h>

#define DIM 2

int main(int argc, char **argv)
{

    char *file = "/home/diogenes/dev/source/file";
    char *word = "ad";
    char buff[DIM];
    char buff_out[DIM];
    int readed, i, count, j = 0;
    int wordLen = strlen(word);
    char tmp_buff[wordLen];

    int fd = open(file, O_RDONLY);

    while ((readed = read(fd, buff, DIM)) > 0)
    {
        for (i = 0; i < readed; i++)
        {
            if (word[j] == buff[i])
            {
                tmp_buff[j] = buff[i];
                for (; j < wordLen && i < readed; ++j)
                {
                    if (word[j] == buff[i])
                    {
                        tmp_buff[j] = buff[i++];
                    }
                    else
                    {
                        write(0, tmp_buff, strlen(tmp_buff));
                        j = 0;
                        break;
                    }
                }
                if (j == wordLen)
                {
                    j = 0;
                }
            }
            write(0, &buff[i], 1);
        }
    }
    close(fd);
}