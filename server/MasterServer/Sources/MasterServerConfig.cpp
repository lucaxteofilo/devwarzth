#include "r3dPCH.h"
#include "r3d.h"

#include "MasterServerConfig.h"
#include "MasterGameServer.h"
#include "MasterUserServer.h"

CMasterServerConfig* gServerConfig;

static const char* configRentFile = "MasterServer_Rent.cfg";
static const char* configFile = "MasterServer.cfg";
const char* configRentFileFullDir = "";
CMasterServerConfig::CMasterServerConfig()
{	
	const char* group      = "MasterServer";

	if(_access(configFile, 0) != 0) {
		r3dOutToLog("can't open config file %s\n", configFile);
	}
	maxrent = r3dReadCFG_I(configFile, group, "MaxRent",0);
	masterPort_  = r3dReadCFG_I(configFile, group, "masterPort", SBNET_MASTER_PORT);
	clientPort_  = r3dReadCFG_I(configFile, group, "clientPort", GBNET_CLIENT_PORT);
	masterCCU_   = r3dReadCFG_I(configFile, group, "masterCCU",  3000);

	supervisorCoolDownSeconds_ = r3dReadCFG_F(configFile, group, "supervisorCoolDownSeconds",  15.0f);

#define CHECK_I(xx) if(xx == 0)  r3dError("missing %s value in %s", #xx, configFile);
#define CHECK_S(xx) if(xx == "") r3dError("missing %s value in %s", #xx, configFile);
	CHECK_I(masterPort_);
	CHECK_I(clientPort_);
#undef CHECK_I
#undef CHECK_S

	serverId_    = r3dReadCFG_I(configFile, group, "serverId", 0);
	if(serverId_ == 0)
	{
		MessageBox(NULL, "you must define serverId in MasterServer.cfg", "", MB_OK);
		r3dError("no serverId");
	}
	if(serverId_ > 255 || serverId_ < 1)
	{
		MessageBox(NULL, "bad serverId", "", MB_OK);
		r3dError("bad serverId");
	}
	r3dOutToLog("MaxRentGames %d\n",maxrent);
	configRentFileFullDir = r3dReadCFG_S(configFile, "MasterServer", "RentFullDir",  "");
	if (configRentFileFullDir[0] == 0)
		r3dError("missing RentFullDir value in %s\n",configFile);

	r3dOutToLog("RentFullDir:%s\n",configRentFileFullDir);
	//r3dWriteCFG_S(configRentFileFullDir,"RentGamesTest","pwd","paspas45");
	LoadConfig();
	//LoadRentConfig();
	return;
}
void CMasterServerConfig::LoadRentConfig()
{
	bool havesupers = false;
	bool havesupers2 = false;
	for(CMasterGameServer::TSupersList::const_iterator it = gMasterGameServer.supers_.begin();
		it != gMasterGameServer.supers_.end();
		++it)
	{
		const CServerS* super = it->second;
		if (super->region_ == GBNET_REGION_Europe)
		{
			havesupers = true;
		}

		if (super->region_ == GBNET_REGION_184)
		{
			havesupers2 = true;
		}
	}

	if (!havesupers || !havesupers2)
	{
		return;
	}
	//r3dCloseCFG_Cur();

	//r3dOutToLog("!!!!!!!!! THIS SERVER NOT FOR FUCKER ElectroZ FUCKYOU POON !!!!!!\n");
	if(_access(configRentFile, 0) != 0)
	{
		//r3dOutToLog("can't open rent config file %s\n", configRentFile);
		return;
	}


	//r3dOutToLog("Read RentServer Config...\n");
	//r3dWriteCFG_S(configRentFile,"RentGames34133","name","123456789");
	//   r3dOutToLog("open rent config file %s\n", configRentFile);
	__int64 secs1 = _time64(&secs1);
	// r3dOutToLog("Current Time %d\n",secs1);
	for(int i=30000; i<40000; i++)
	{
		char group[128];
		sprintf(group, "RentGames%d", i+1);

		char map[512] = "";
		char maxplayer1[512];
		int maxplayer;
		char pwdchar[512];
		char name[512];
		char expirein[512];
		char period1[512];
		char ownerc[512];
		int owner;
		int mapid;
		int expire;
		int period;
		r3dscpy(expirein, r3dReadCFG_S(configRentFile, group, "expire", ""));
		sscanf(expirein, "%d", &expire);
		r3dscpy(period1, r3dReadCFG_S(configRentFile, group, "period", ""));
		sscanf(period1, "%d", &period);
		r3dscpy(name, r3dReadCFG_S(configRentFile, group, "name", ""));
		if(name[0] == 0) continue; // no config
		__int64 secs1 = _time64(&secs1);
		if (secs1 > expire) // expired server
		{
			// r3dOutToLog("RentServer : %d expired (%d , %d sec))\n", i+1,secs1,expire - secs1);
			continue;
		}
		r3dscpy(pwdchar, r3dReadCFG_S(configRentFile, group, "pwd", ""));
		//r3dOutToLog("RentServer : Name : %s , Pwd = %s , setting gameinfo config...\n" , name , pwdchar);
		r3dscpy(maxplayer1, r3dReadCFG_S(configRentFile, group, "maxplayer", ""));
		r3dscpy(map, r3dReadCFG_S(configRentFile, group, "mapid", ""));
		r3dscpy(ownerc, r3dReadCFG_S(configRentFile, group, "ownercustomerid", ""));
		sscanf(maxplayer1, "%d", &maxplayer);
		sscanf(ownerc, "%d", &owner);
		sscanf(map, "%d", &mapid);
		GBGameInfo ginfo;
		ginfo.expirein = expire;
		sprintf(ginfo.name,name);
		sprintf(ginfo.pwdchar,pwdchar);
		ginfo.mapId = mapid;

		int gamecount = 0;
		int gamecount2 = 0;
		int gamecount3 = 0;
		int gamecount4 = 0;
		for(CMasterGameServer::TSupersList::const_iterator it = gMasterGameServer.supers_.begin();
			it != gMasterGameServer.supers_.end();
			++it)
		{
			const CServerS* super = it->second;
			if (super->region_ == GBNET_REGION_Europe)
			{
				gamecount = super->GetExpectedGames();
			}

			if (super->region_ == GBNET_REGION_Europe2)
			{
				gamecount2 = super->GetExpectedGames();
			}

			if (super->region_ == GBNET_REGION_US_East)
			{
				gamecount3 = super->GetExpectedGames();
			}
			if (super->region_ == GBNET_REGION_Europe1)
			{
				gamecount4 = super->GetExpectedGames();
			}
		}
		if (gamecount < 28)
		{
			ginfo.region = GBNET_REGION_Europe;
		}
		else if (gamecount2 < 22)
		{
			ginfo.region = GBNET_REGION_Europe2;
		}
		else
		{
			ginfo.region = GBNET_REGION_184;
		}
		/*else if (gamecount2 < 22)
		{
			ginfo.region = GBNET_REGION_Europe2;
		}
		else if (gamecount3 < 28)
		{
            ginfo.region = GBNET_REGION_US_East;
		}
		else
		{
            ginfo.region = GBNET_REGION_Europe1;
		}*/
		ginfo.maxPlayers = maxplayer;
		ginfo.ispass = true;
		ginfo.isexited = false;
		ginfo.ispre = false;
		ginfo.ownercustomerid = owner;

		// after set server config finished let's start server
		ginfo.gameServerId = i+1; // set gameserverid by RentGame
		if(gMasterGameServer.IsGameServerIdStarted(ginfo.gameServerId))
			continue;
		//r3dOutToLog("RentServer : starting... %d\n" , ginfo.gameServerId); // TimeLeft
		//r3dOutToLog("RentServer : id %d (Time Left %d sec(%d), %d(%d) min, %d(%d) hrs,(Period %d days)\n",ginfo.gameServerId,expire - secs1 , (expire - secs1) /60 , ((expire - secs1) /60) /60, period);
		CMSNewGameData ngd(ginfo, "", 0);
		DWORD ip;
		DWORD port;
		__int64 sessionId;
		gMasterGameServer.CreateNewGame(ngd, &ip, &port, &sessionId);
		// COMPLETED FUNCTION HA HA HA !!!!
	}
}
void CMasterServerConfig::LoadConfig()
{
	r3dCloseCFG_Cur();

	numPermGames_ = 0;

	/*  FILE* f = fopen("MasterServer_Rent.cfg", "a");
	if(f) fprintf(f, "Test1\n");
	if(f) fprintf(f, "Test2\n");
	if(f) fclose(f);*/
	maxrent = r3dReadCFG_I(configFile, "MasterServer", "MaxRent",0);
	LoadPermGamesConfig();
	Temp_Load_WarZGames();
}

void CMasterServerConfig::Temp_Load_WarZGames()
{
	char group[128];
	sprintf(group, "WarZGames");

	int numGames   = r3dReadCFG_I(configFile, group, "numGames", 0);
	int maxPlayers = r3dReadCFG_I(configFile, group, "maxPlayers", 32);

	r3dOutToLog("WarZ %d games, %d players each\n", numGames, maxPlayers);

	for(int i=0; i<numGames; i++)
	{
		GBGameInfo ginfo;
		ginfo.mapId      = GBGameInfo::MAPID_WZ_Colorado;
		ginfo.mapId      = GBGameInfo::MAPID_UB_Cliffside;
		ginfo.mapId      = GBGameInfo::MAPID_UB_CaliWood;
		ginfo.mapId      = GBGameInfo::MAPID_UB_Valley;
		ginfo.mapId      = GBGameInfo::MAPID_UB_Area51;
		ginfo.mapId      = GBGameInfo::MAPID_UB_CryZ;
		ginfo.mapId      = GBGameInfo::MAPID_UB_Terra;
		ginfo.mapId      = GBGameInfo::MAPID_UB_Mega;
		ginfo.maxPlayers = maxPlayers;

		sprintf(ginfo.name, "US Server %03d", i + 1);
		AddPermanentGame(10000 + i, ginfo, GBNET_REGION_US_West);

		sprintf(ginfo.name, "EU Server %03d", i + 1);
		AddPermanentGame(20000 + i, ginfo, GBNET_REGION_Europe);
	}
}

void CMasterServerConfig::LoadPermGamesConfig()
{
	numPermGames_ = 0;

	//#ifdef _DEBUG
	//  r3dOutToLog("Permanet games disabled in DEBUG");
	//  return;
	//#endif

	for(int i=0; i<512; i++)
	{
		char group[128];
		sprintf(group, "PermGame%d", i+1);

		char map[512] = "";
		char data[512] = "";
		char pwdchar[512];
		char name[512];
		int ispwd;
		int ispre2;
		char ispwd1[512];
		char ispre1[512];
		int isTop = 0;
		r3dscpy(ispwd1,  r3dReadCFG_S(configFile, group, "ispwd", ""));
		r3dscpy(ispre1,  r3dReadCFG_S(configFile, group, "ispre", ""));
		r3dscpy(map,  r3dReadCFG_S(configFile, group, "map", ""));
		r3dscpy(data, r3dReadCFG_S(configFile, group, "data", ""));
		r3dscpy(name, r3dReadCFG_S(configFile, group, "name", ""));
		//int ispassword = r3dReadCFG_I(configFile, group, "ispassword", 32);
		r3dscpy(pwdchar, r3dReadCFG_S(configFile, group, "pwd", ""));
		if(name[0] == 0)
			sprintf(name, "PermGame%d", i+1);

		if(*map == 0)
			continue;

		isTop = r3dReadCFG_I(configFile, group, "isTop", 0);
		sscanf(ispwd1, "%d", &ispwd);
		sscanf(ispre1, "%d", &ispre2);
		bool ispre = false;

		if (ispre2 == 1)
		{
			ispre = true;
		}

		sscanf(ispwd1, "%d", &ispwd);
		bool ispass = false;

		if (ispwd == 1)
		{
			ispass = true;
		}

		bool isfarm = false;

		if (ispass && ispre)
		{
			ispass = false;
			ispre = false;
			isfarm = true;
		}

		if (ispre)
			r3dOutToLog ("ID:%d , Name:%s isPre = 1\n",i,name);

		ParsePermamentGame(i, name, map, data,pwdchar,ispass,ispre,isfarm , r3dReadCFG_I(configFile, group, "ownercustomerid", 1013668),isTop);
	}

	return;  
}

static int StringToGBMapID(char* str)
{
	if(stricmp(str, "MAPID_WZ_Colorado") == 0)
		return GBGameInfo::MAPID_WZ_Colorado;
	if(stricmp(str, "MAPID_UB_Cliffside") == 0)
		return GBGameInfo::MAPID_UB_Cliffside;
	if(stricmp(str, "MAPID_UB_CaliWood") == 0)
		return GBGameInfo::MAPID_UB_CaliWood;
	if(stricmp(str, "MAPID_UB_Valley") == 0)
		return GBGameInfo::MAPID_UB_Valley;
	if(stricmp(str, "MAPID_UB_Area51") == 0)
		return GBGameInfo::MAPID_UB_Area51;
	if(stricmp(str, "MAPID_UB_CryZ") == 0)
		return GBGameInfo::MAPID_UB_CryZ;
    if(stricmp(str, "MAPID_UB_Terra") == 0)
		return GBGameInfo::MAPID_UB_Terra;
	if(stricmp(str, "MAPID_UB_Mega") == 0)
		return GBGameInfo::MAPID_UB_Mega;
	if(stricmp(str, "MAPID_Editor_Particles") == 0)
		return GBGameInfo::MAPID_Editor_Particles;
	if(stricmp(str, "MAPID_ServerTest") == 0)
		return GBGameInfo::MAPID_ServerTest;


	r3dError("bad GBMapID %s\n", str);
	return 0;
}

static EGBGameRegion StringToGBRegion(const char* str)
{
	if(stricmp(str, "GBNET_REGION_US_West") == 0)
		return GBNET_REGION_US_West;
	if(stricmp(str, "GBNET_REGION_US_East") == 0)
		return GBNET_REGION_US_East;
	if(stricmp(str, "GBNET_REGION_Europe") == 0)
		return GBNET_REGION_Europe;
	if(stricmp(str, "GBNET_REGION_Europe1") == 0)
		return GBNET_REGION_Europe1;
	if(stricmp(str, "GBNET_REGION_Europe2") == 0)
		return GBNET_REGION_Europe2;
	//if(stricmp(str, "GBNET_REGION_Europe") == 0)
	//	return GBNET_REGION_Europe;
	if(stricmp(str, "GBNET_REGION_Russia") == 0)
		return GBNET_REGION_Russia;
	if(stricmp(str, "GBNET_REGION_184") == 0)
		return GBNET_REGION_184;
	if(stricmp(str, "GBNET_REGION_185") == 0)
		return GBNET_REGION_185;
	if(stricmp(str, "GBNET_REGION_186") == 0)
		return GBNET_REGION_186;
	if(stricmp(str, "GBNET_REGION_187") == 0)
		return GBNET_REGION_187;
	if(stricmp(str, "GBNET_REGION_188") == 0)
		return GBNET_REGION_188;
	if(stricmp(str, "GBNET_REGION_s1") == 0)
		return GBNET_REGION_s1;
	if(stricmp(str, "GBNET_REGION_s2") == 0)
		return GBNET_REGION_s2;
	if(stricmp(str, "GBNET_REGION_s3") == 0)
		return GBNET_REGION_s3;
	if(stricmp(str, "GBNET_REGION_s4") == 0)
		return GBNET_REGION_s4;

	r3dError("bad GBGameRegion %s\n", str);
	return GBNET_REGION_Unknown;
}

void CMasterServerConfig::ParsePermamentGame(int gameServerId, const char* name, const char* map, const char* data,const char* pwdchar,bool ispass,bool ispre,bool isfarm , int owner, int isTop)
{
	char mapid[128];
	char maptype[128];
	char region[128];
	int minGames;
	int maxGames;
	if(5 != sscanf(map, "%s %s %s %d %d", mapid, maptype, region, &minGames, &maxGames)) {
		r3dError("bad map format: %s\n", map);
	}

	int maxPlayers;
	int minLevel = 0;
	int maxLevel = 0;
	if(3 != sscanf(data, "%d %d %d", &maxPlayers, &minLevel, &maxLevel)) {
		r3dError("bad data format: %s\n", data);
	}

	GBGameInfo ginfo;
	ginfo.ownercustomerid = owner;
	ginfo.mapId        = StringToGBMapID(mapid);
	ginfo.maxPlayers   = maxPlayers;
	ginfo.expirein = (int)999999999999;
	//ginfo.maxPlayers   = maxPlayers; สัสทำไมมันซํ้ากันวะ
	//if (pwd == "")
	//{
	//	   r3dscpy(ginfo.IsPassword,NULL);
	//}
	//else
	//{
	// r3dscpy(ginfo.IsPassword,pwd);
	ginfo.ispass = ispass;
	ginfo.ispre = ispre;
	ginfo.isfarm = isfarm;
	r3dscpy(ginfo.name, name);
	r3dscpy(ginfo.pwdchar, pwdchar);
	ginfo.isTop = false;

	if (isTop)
		ginfo.isTop = true;
	//  }
	r3dOutToLog("permgame: ID:%d, %s, %s,%s\n", 
		gameServerId, name, mapid,pwdchar);

	EGBGameRegion eregion = StringToGBRegion(region);
	AddPermanentGame(gameServerId, ginfo, eregion);
}
void CMasterServerConfig::SetRentPwd(int gameServerId, const char* pwd)
{
	char group[512];
	sprintf(group,"RentGames%d",gameServerId);
	r3dOutToLog("SetRentPwd gameServerId%s  cfg %s\n",group,r3dReadCFG_S(configFile, "MasterServer", "RentFullDir",  ""));
	r3dWriteCFG_S(r3dReadCFG_S(configFile, "MasterServer", "RentFullDir",  ""),group,"pwd",pwd);
	gMasterGameServer.SendKillGame(gameServerId);
}
void CMasterServerConfig::AddPermanentGame(int gameServerId, const GBGameInfo& ginfo, EGBGameRegion region)
{
	r3d_assert(numPermGames_ < R3D_ARRAYSIZE(permGames_));
	permGame_s& pg = permGames_[numPermGames_++];

	r3d_assert(gameServerId);
	pg.ginfo.ownercustomerid = 0;
	pg.ginfo = ginfo;
	pg.ginfo.gameServerId = gameServerId;
	pg.ginfo.region       = region;

	return;
}

void CMasterServerConfig::AddRentGame(int customerid , const char* name , const char* pwd , int mapid , int slot , int period)
{
	static std::vector<const CServerS*> supers;
	supers.clear();
	for(CMasterGameServer::TSupersList::const_iterator it = gMasterGameServer.supers_.begin(); it != gMasterGameServer.supers_.end(); ++it)
		supers.push_back(it->second);

	const CServerS* super = supers[0];

	__int64 secs1 = _time64(&secs1);
	GBGameInfo ginfo;
	ginfo.mapId = mapid;
	ginfo.region = GBNET_REGION_Europe;
	ginfo.maxPlayers = slot;
	ginfo.gameServerId = (int)u_GetRandom(30000,40000);
	bool isAl = false;

	char group[128];
	sprintf(group, "RentGames%d", ginfo.gameServerId);

	if (r3dReadCFG_S(configRentFile, group, "name", "")[0] != 0) // it's already have gameserverid.
		isAl = true;

	if(gMasterGameServer.IsGameServerIdStarted(ginfo.gameServerId) || isAl)
	{
		r3dOutToLog("RentServer id %d already started i will rerun function..\n",ginfo.gameServerId);
		AddRentGame(customerid,name,pwd,mapid,slot,period);
		return;
	}
	ginfo.ownercustomerid = customerid; // !!!!!!! MUST SET CUSTOMERID FOR MY SERVER SCREEN !!!!!!!!!!
	r3dscpy(ginfo.name,name);
	ginfo.ispass = true; // private server
	ginfo.ispre = false;
	ginfo.isexited = false;
	r3dscpy(ginfo.pwdchar,pwd);
	ginfo.expirein = (int)secs1 + (period * 86400);

	// save config before CreateNewGame()
	FILE* f = fopen(configRentFile, "a"); // open and continute write
	r3dOutToLog("RentServer : write rent config..\n");
	if (!f) return; // if save failed we will cancel function
	if (f)
	{
		fprintf(f, "[RentGames%d]\n" , ginfo.gameServerId);
		fprintf(f, "name = %s\n" , name);
		fprintf(f, "mapid = %d\n" , mapid);
		fprintf(f, "pwd = %s\n" , pwd);
		fprintf(f, "maxplayer = %d\n" , slot);
		fprintf(f, "ownercustomerid = %d\n" , customerid);
		fprintf(f, "expire = %d\n" , secs1 + (period * 86400)); // now time + period(day) * 86400( = 1 day in sec)
		fprintf(f, "period = %d\n" , period);
		fprintf(f, "create = %d\n" , secs1);
		fprintf(f, "region = %d\n" , 10);
		fprintf(f, "\n" , 0); // space
		fclose(f); // close file
	}
	// after save config finished start CreateNewGame() now
	CMSNewGameData ngd(ginfo, "", 0);
	//DWORD ip;
	//DWORD port;
	//__int64 sessionId;
	r3dOutToLog("RentServer : Starting..\n");
	//r3dOutToLog("RentServer : id %d (Time Left %d sec(%d), %d(%d) min, %d(%d) hrs,(Period %d days)\n",ginfo.gameServerId,expire - secs1 , (expire - secs1) /60 , ((expire - secs1) /60) /60, period);
	//gMasterGameServer.CreateNewGame(ngd, &ip, &port, &sessionId);
}