#include "nrf24L01.h"
#include "global_var.h"
#include "uart.h"

//******************************************************************************************
uchar  bdata sta;   //״̬��־
sbit   RX_DR =sta^6;
sbit   TX_DS =sta^5;
sbit   MAX_RT =sta^4;
//*********************************************NRF24L01*************************************
#define TX_ADR_WIDTH    5    // 5 uints TX address width
#define RX_ADR_WIDTH    5    // 5 uints RX address width
#define TX_PLOAD_WIDTH  sizeof(nrf_pkt_t)   // 32 uints TX payload
#define RX_PLOAD_WIDTH  sizeof(nrf_pkt_t)   // 32 uints TX payload

//uchar const TX_ADDRESS[TX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //���ص�ַ
//uchar const RX_ADDRESS[RX_ADR_WIDTH]= {0x34,0x43,0x10,0x10,0x01}; //���յ�ַ

//����ĵ�ַ��������Ķ� ֻ�豣֤���ͽ���һ�¼���
uchar const TX_ADDRESS[TX_ADR_WIDTH]= {0x01,0x12,0x34,0x56,0x78}; //���ص�ַ
uchar const RX_ADDRESS[RX_ADR_WIDTH]= {0x01,0x12,0x34,0x56,0x78}; //���յ�ַ

void nRF24L01_init(void) {
	CE=0;
	SCK=0;
	CSN=1;
	return;
}

/************************************IO ��ģ��SPI���� ����************************************************/
uchar SPI_RW(uchar byte) {
	uchar bit_ctr;
	for(bit_ctr=0;bit_ctr<8;bit_ctr++) {
		MOSI=(byte&0x80);
		byte=(byte<<1);
		SCK=1;
		byte|=MISO;
		//delay(150);
		SCK=0;
	}
	return(byte);
}
uchar SPI_RW_Reg  (uchar  reg,uchar value) // ��Ĵ���REGдһ���ֽڣ�ͬʱ����״̬�ֽ�
{
	uchar status;
	CSN=0;
	status=SPI_RW(reg);
	SPI_RW(value);
	CSN=1;
	return(status);
}
uchar SPI_Read (uchar  reg )
{
	uchar reg_val;
	CSN=0;
	SPI_RW(reg);
	reg_val=SPI_RW(0);
	CSN=1;
	return(reg_val);
}
uchar SPI_Write_Buf(uchar reg, uchar *pBuf, uchar bytes)
{
	uchar status,byte_ctr;
	CSN = 0;                   // Set CSN low, init SPI tranaction
	status = SPI_RW(reg);    // Select register to write to and read status byte
	for(byte_ctr=0; byte_ctr<bytes; byte_ctr++) // then write all byte in buffer(*pBuf)
		SPI_RW(*pBuf++);
	CSN = 1;                 // Set CSN high again
	return(status);          // return nRF24L01 status byte
}
 
/*******************************��*****��*****ģ*****ʽ*****��*****��*************************************/
void TX_Mode(void) {
	CE=0;
	SPI_RW_Reg(FLUSH_TX,0x00);
	SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
	SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...1a
	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40 
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:1Mbps, LNA:HCURR
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ2�ֽ�
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);
	CE=1;
	delay_ms(100);
}

unsigned char nRF24L01_TxPacket(unsigned char * tx_buf) 
{
	u8 status, retval;
	CE=0;   //StandBy Iģʽ 
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // װ�ؽ��ն˵�ַ
	SPI_RW_Reg(FLUSH_TX,0x00);
	SPI_Write_Buf(WR_TX_PLOAD, tx_buf, TX_PLOAD_WIDTH);     // װ������ 
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0e);      // IRQ�շ�����ж���Ӧ��16λCRC��������
	CE=1;   //�ø�CE���������ݷ���
	delay_ms(150);
		
	status = SPI_Read(READ_REG +  STATUS);
	if(0x20 & status) 
	{
		SPI_RW_Reg(WRITE_REG + STATUS,status);
		retval = 1;
	} 
	else 
	{
		SPI_RW_Reg(WRITE_REG + STATUS,status);
		retval = 0;
	} 
	return retval;
}
/****************************************************************************************************/
/*******************************��*****��*****ģ*****ʽ*****��*****��********************************/
/****************************************************************************************************/
uchar SPI_Read_Buf(uchar reg, uchar *pBuf, uchar uchars)
{
	uchar status,uchar_ctr;
 
	CSN = 0;                      // Set CSN low, init SPI tranaction
	status = SPI_RW(reg);         // Select register to write to and read status uchar
 
	for(uchar_ctr=0;uchar_ctr<uchars;uchar_ctr++)
		pBuf[uchar_ctr] = SPI_RW(0);    // 
 
	CSN = 1;       
	return(status);                    // return nRF24L01 status uchar
}

/****************************************************************************************************/
/*������void SetRX_Mode(void)
/*���ܣ����ݽ������� 
/****************************************************************************************************/

void SetRX_Mode(void)
{
	CE=0;
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������
	CE = 1; 
	delay_ms(10);
}  

/******************************************************************************************************/
/*������unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
/*���ܣ����ݶ�ȡ�����rx_buf���ջ�������
/******************************************************************************************************/
unsigned char nRF24L01_RxPacket(unsigned char* rx_buf)
{
	unsigned char revale=0;
	sta=SPI_Read(STATUS); // ��ȡ״̬�Ĵ������ж����ݽ���״��
	if(RX_DR)    // �ж��Ƿ���յ�����
	{
		CE = 0;    //SPIʹ��
		SPI_Read_Buf(RD_RX_PLOAD,rx_buf,RX_PLOAD_WIDTH);// read receive payload from RX_FIFO buffer
		revale =1;   //��ȡ������ɱ�־
	}
	SPI_RW_Reg(WRITE_REG+STATUS,sta);   //���յ����ݺ�RX_DR,TX_DS,MAX_PT���ø�Ϊ1��ͨ��д1������жϱ�־
	
	CE=0;
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0f);   		// IRQ�շ�����ж���Ӧ��16λCRC	��������
	CE = 1; 
	delay_ms(10);
	return revale;
}
/****************************************************************************************************/
/*������void RX_Mode(void)
/*���ܣ����ݽ������� 11
/****************************************************************************************************/
void RX_Mode(void)
{
	CE=0;
	SPI_RW_Reg(FLUSH_RX,0x00);
	//SPI_Write_Buf(WRITE_REG + TX_ADDR, TX_ADDRESS, TX_ADR_WIDTH);    // Writes TX_Address to nRF24L01
	SPI_Write_Buf(WRITE_REG + RX_ADDR_P0, TX_ADDRESS, TX_ADR_WIDTH); // RX_Addr0 same as TX_Adr for Auto.Ack
   
	SPI_RW_Reg(WRITE_REG + EN_AA, 0x01);      // Enable Auto.Ack:Pipe0
	SPI_RW_Reg(WRITE_REG + EN_RXADDR, 0x01);  // Enable Pipe0
   //SPI_RW_Reg(WRITE_REG + SETUP_RETR, 0x1a); // 500us + 86us, 10 retrans...1a
	SPI_RW_Reg(WRITE_REG + RF_CH, 40);        // Select RF channel 40
	SPI_RW_Reg(WRITE_REG + RX_PW_P0, RX_PLOAD_WIDTH); //���ý������ݳ��ȣ���������Ϊ2�ֽ�
	SPI_RW_Reg(WRITE_REG + RF_SETUP, 0x07);   // TX_PWR:0dBm, Datarate:1Mbps, LNA:HCURR
	SPI_RW_Reg(WRITE_REG + CONFIG, 0x0F);
	CE=1;
	delay_ms(130);
}

void nrf_tx_mode_init(void) {
	nRF24L01_init();
	TX_Mode();
}

void nrf_rx_mode_init(void) {
	nRF24L01_init();
	RX_Mode();
}