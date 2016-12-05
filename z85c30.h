class z85c30
{
private:
	unsigned char d[16];
	uint8_t cr;
	bool tx_full;

public:
    z85c30();
	~z85c30();

	uint8_t ser_command_read();
	uint8_t ser_data_read();

	void ser_command_write(uint8_t data);
	void ser_data_write(uint8_t data);
};
