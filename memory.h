class memory
{
private:
	unsigned char *pm;
	int len;

public:
	memory(int size);
	~memory();

	bool read_32b(int offset, int *data);
	bool write_32b(int offset, int data);
};
