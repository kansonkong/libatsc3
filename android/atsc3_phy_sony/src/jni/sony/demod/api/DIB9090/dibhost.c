#include "dibcommon.h"
#include "dibbusdescriptor.h"
#include "dibbusadapter.h"
#include "dibhost.h"



//#define DEBUG_DATA

//#define DEBUG_LOG

#ifdef DEBUG_LOG
FILE* fp;    
#endif

struct i2c_state {
    void *priv;
};

static int host_i2c_xfer(struct dibDataBusClient *client, struct dibDataBusAccess *msg)
{
	Dword error;
	Byte i2cadd;
	Endeavour* endeavour_dib;
	unsigned char endeavour_dib_I2C_bus;
	unsigned char endeavour_dib_chip_idx;

#ifdef DEBUG_LOG
	Byte i;
	Byte wLen;
	Byte rLen;
	Byte debug_wBuf[256];
	Byte debug_rBuf[256];
#endif

	endeavour_dib = client->host->endeavour_dib;
	endeavour_dib_chip_idx = client->host->endeavour_dib_chip_idx;
	endeavour_dib_I2C_bus = client->host->endeavour_dib_I2C_bus;

    //struct i2c_state *st = client->host->priv;

#ifdef DEBUG_DATA
    dbgp("i2c_%s: (addr: %02x): ",msg->rx == NULL ? "wr" : "rd",data_bus_access_device_id(client, msg) >> 1);
    dbg_bufdump(msg->tx,msg->txlen);
    if (msg->rx != NULL && msg->rxlen != 0) {
        dbgp(" - ");
        dbg_bufdump(msg->rx,msg->rxlen);
    }
    dbgp("\n");
#endif

	i2cadd = (Byte)(client->device_id);

#ifdef DEBUG_LOG
	wLen = (Byte)(msg->txlen);
	rLen = (Byte)(msg->rxlen);
	for(i = 0; i < wLen; i++)
		debug_wBuf[i] = (msg->tx)[i];
	for(i = 0; i < rLen; i++)
		debug_rBuf[i] = (msg->rx)[i];
	fopen_s(&fp,"dumpDIB9090.txt", "a");
	fprintf(fp, "I2C Add : 0x%02X\n", i2cadd);
	fprintf(fp, "wLen : %d, rLen : %d\n", wLen, rLen);
	for(i = 0; i < wLen; i++)
		fprintf(fp, "wBuf[%d] = 0x%02X\n", i, debug_wBuf[i]);
#endif


	if (msg->rx == NULL || msg->rxlen == 0)		//write
	{
		error = IT9300_writeGenericRegisters(endeavour_dib, endeavour_dib_chip_idx, endeavour_dib_I2C_bus, i2cadd, (Byte)(msg->txlen), (Byte*)(msg->tx));

#ifdef DEBUG_LOG
		fprintf(fp, "Error code : 0x%08x\n", error);
		fclose(fp);
#endif

		if(error) return DIB_RETURN_ERROR;		
    }
    else										//read
	{
		error = IT9300_writeGenericRegisters(endeavour_dib, endeavour_dib_chip_idx, endeavour_dib_I2C_bus, i2cadd, (Byte)msg->txlen, (Byte*)msg->tx);

#ifdef DEBUG_LOG
		fprintf(fp, "Error code : 0x%08x\n", error);
		fclose(fp);
#endif

		if(error) return DIB_RETURN_ERROR;

		error = IT9300_readGenericRegisters(endeavour_dib, endeavour_dib_chip_idx, endeavour_dib_I2C_bus, i2cadd, (Byte)(msg->rxlen), (Byte*)msg->rx);

#ifdef DEBUG_LOG
		fprintf(fp, "Error code : 0x%08x\n", error);
		fclose(fp);
#endif

		if(error) return DIB_RETURN_ERROR;	
    }

#ifdef DEBUG_LOG
	rLen = (Byte)(msg->rxlen);
	for(i = 0; i < rLen; i++)
		debug_rBuf[i] = (msg->rx)[i];
	fopen_s(&fp,"dumpDIB9090.txt", "a");
	for(i = 0; i < rLen; i++)
		fprintf(fp, "rBuf[%d] = 0x%02X\n", i, debug_rBuf[i]);
	fprintf(fp, "----------------------------------\n");
	fclose(fp);
#endif

	return DIB_RETURN_SUCCESS;
}

void host_i2c_release(struct dibDataBusHost *i2c_adap)
{
    struct i2c_state *state = (struct i2c_state*)(i2c_adap->priv);
    DibDbgPrint("-I-  closing I2C\n");

    MemFree(state, sizeof(struct i2c_state));
}

struct dibDataBusHost * host_i2c_interface_attach(Endeavour* endeavour, unsigned char i2c_bus, unsigned char chip_idx, void *priv)
{
    struct i2c_state *state = (struct i2c_state*)(MemAlloc(sizeof(struct i2c_state)));
    struct dibDataBusHost *i2c_adap = (struct dibDataBusHost*)(MemAlloc(sizeof(struct dibDataBusHost)));

    data_bus_host_init(i2c_adap, DATA_BUS_I2C, host_i2c_xfer, state);


	i2c_adap->endeavour_dib = endeavour;
	i2c_adap->endeavour_dib_I2C_bus = i2c_bus;
	i2c_adap->endeavour_dib_chip_idx = chip_idx;
    state->priv = priv;


    return i2c_adap;
/*
    goto free_mem;
free_mem:
    MemFree(state,sizeof(struct i2c_state));

    return NULL;
*/
}

