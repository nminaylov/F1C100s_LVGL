#include <errno.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/times.h>
#include <sys/unistd.h>

#define HEAP_SIZE		0x00100000

// errno
#undef errno
extern int errno;

static char heap[HEAP_SIZE];

/* pointer to current position in heap */
static char* _cur_brk = heap;


extern void _putchar(char c);

// close - закрытие файла - возвращаем ошибку
int _close(int file)
{
    return -1;
}

/*
 fstat - состояние открытого файла
 */
int _fstat(int file, struct stat *st)
{
    return 0;
}

/*
 isatty - является ли файл терминалом.
 */
int _isatty(int file)
{
	return 0;
}

/*
 lseek - установить позицию в файле
 */
int _lseek(int file, int ptr, int dir)
{
    return 0;
}

/*
 sbrk - увеличить размер области данных, использутся для malloc
 */
caddr_t _sbrk_r(struct _reent* r, int incr)
{
	char* _old_brk = _cur_brk;

	if ((_cur_brk + incr) > (heap + HEAP_SIZE))
	{
		uint8_t i;

		char* msg = "HEAP FULL!\r\n";

		for (i = 0; i < strlen(msg); i++)
		{
			_putchar(msg[i]);
		}

		errno = ENOMEM;
		return (void *)-1;
	}

	_cur_brk += incr;

	return (caddr_t)_old_brk;
}

/*
 read - чтение из файла, у нас пока для чтения есть только stdin
 */

int _read(int file, char *ptr, int len)
{
    return 0;
}

/*
 write - запись в файл - у нас есть только stderr/stdout
 */
int _write(int file, char *ptr, int len)
{
	for (uint32_t i = 0; i < len; i++)
	{
		if (ptr[i] == '\n')
		{
			_putchar('\r');
		}
		_putchar(ptr[i]);
	}
	return len;
}

