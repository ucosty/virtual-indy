class memory
{
private:
	unsigned char *pm;
	int len;

public:
	memory(int size);
	~memory();

	bool read_32b(int offset, int *data);
	bool read_16b(int offset, int *data);
	bool read_8b(int offset, int *data);
	bool write_32b(int offset, int data);
	bool write_16b(int offset, int data);
	bool write_8b(int offset, int data);
};
