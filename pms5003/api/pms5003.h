#ifndef _PMS_5003_H_
#define _PMS_5003_H_


typedef struct pms5003_s_t
{
	//PM1.0 浓度(CF=1，标准颗粒物) 单位μ g/m3
	uint16_t cfpm1p0;
	//PM2.5 浓度(CF=1，标准颗粒物) 单位μ g/m3
	uint16_t cfpm2p5;
	//PM10 浓度(CF=1，标准颗粒物) 单位μ g/m3
	uint16_t cfpm10;
	//PM1.0 浓度(大气环境下) 单位μ g/m3
	uint16_t stdpm1p0;
	//PM2.5 浓度(大气环境下) 单位μ g/m3
	uint16_t stdpm2p5;
	//PM10 浓度 (大气环境下) 单位μ g/m3
	uint16_t stdpm10;
	//0.1 升空气中直径在 0.3um 以上 颗粒物个数
	uint16_t pm0p3count;
	//0.1 升空气中直径在 0.5um 以上 颗粒物个数
	uint16_t pm0p5count;
	//0.1 升空气中直径在 1.0um 以上 颗粒物个数
	uint16_t pm1p0count;
	//0.1 升空气中直径在 2.5um 以上 颗粒物个数
	uint16_t pm2p5count;
	//0.1 升空气中直径在 5.0um 以上 颗粒物个数
	uint16_t pm5p0count;
	//0.1 升空气中直径在 10um 以上 颗粒物个数
	uint16_t pm10count;
	//甲醛浓度数值。 注:真实甲醛浓度值=本数值/1000 单位:mg/m3
	uint16_t tvoc;
	//温度 注:真实温度值=本数值/10 单位:°C
	uint16_t temp;
	//湿度 注:真实湿度值=本数值/10 单位:%
	uint16_t hum;
	
}pms5003_s;


//void phrase(char *data,int len);

bool check_sum(char *data,int len);
void phrasepm500s(char *data,int len,struct pms5003_s* pms5003);

#endif