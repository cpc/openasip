static int FIFO_HAS_TOKENS(I)(struct fifo_i32_i *fifo, int n) 
{
	int status;

	FIFO_I_STATUS(I) (0, status);

	return (status >= n);
}

static int FIFO_COUNT_TOKENS(I)(struct fifo_i32_i *fifo) 
{
	int status;

	FIFO_I_STATUS(I) (0, status);

	return status;
}

static int *FIFO_PEEK(I)(struct fifo_i32_i *fifo, int *buffer, int n) 
{
	int i, data;

	for(i = 0; i < n; i++)
	{
		FIFO_I_PEEK(I) (0, data);
		buffer[i] = data;
	}

	return buffer;
}

static int *FIFO_READ(I)(struct fifo_i32_i *fifo, int *buffer, int n) 
{
	int i, data;

	for(i = 0; i < n; i++)
	{
		FIFO_INPUT(I) (0, data);
		buffer[i] = data;
	}

	return buffer;
}

static void FIFO_READ_END(I)(struct fifo_i32_i *fifo, int n)
{
	// intentionally empty
}

