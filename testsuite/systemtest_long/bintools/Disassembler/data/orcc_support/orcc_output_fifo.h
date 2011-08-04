#define SIZE 256

int fifo_token_count(int input);
int fifo_room_count(int output);
void fifo_read(int input, int *buffer, int count);
void fifo_write(int output, int *buffer, int count);

static int FIFO_HAS_ROOM(O)(struct fifo_i32_o *fifo, int n)
{
	int res;
	int status;

	FIFO_O_STATUS(O) (0, status);

	res = (status + n - 1) < SIZE;

	return res;
}

static int *FIFO_WRITE(O)(struct fifo_i32_o *fifo, int *buffer, int n) 
{
	return buffer;
}

static void FIFO_WRITE_END(O)(struct fifo_i32_o *fifo, int *buffer, int n) 
{
	int i;
	int data;

	for(i = 0; i < n; i++) 
	{
		data = buffer[i];
		FIFO_OUTPUT(O) (data);
	}
}
