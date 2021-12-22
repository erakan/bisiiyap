/*
**
** bi$ii yap ~ Winamp plug-in'i kaynak kodu.
** Copyright (c) 2004 Erdinç AKAN 
** erakan(AT)gmail(nokta)com
** http://erdincakan.com
**
** "The Power you need is in the blood, flowing in your veins."
**
** 17 Temmuz 2004, ılık bir Cumartesi akşamı, Bismillah deyip başladım.
**
** Kodun bazı kısımları Ed Zaleski (oddsock(AT)oddsock(nokta)org) ve Justin 
** Frankel tarafından yazılmıştır. Daha ayrintili bilgi icin 
**
** Bu kodlar, GNU General Public License altında salıverilmişlerdir. Kurcalamak 
** serbest, fakat üzerinden para kazanmak, ticari anlamda kullanmak yasahtır.
** Ayrıca kodu kullanabilmek ve dağıtabilmek için bu başlığı silmemeniz 
** gerekir. Bu kodlar size olduğu gibi verilmiştir ve beklenileni yapacağı,
** düzgün şekilde çalışacağı, hayatınızı kolaylaştıracağı, kız arkadaşınızı 
** çalmayacağı kesinlikle garanti edilmemektedir.
**
** Derlemek için Visual C++ 5 üstü gereklidir. MFC'deki bazı class'lar'ı oturup
** yeniden yazmak zor geldiği için böyle. Borland, gcc vs.'de derlenecek hale
** getirmek için, ya IBM'in MFC class'larını bulacaksınız. Yada bu class'ları
** siz yazacaksınız. Vaktim olsa ben yazardım. Sizin vaktiniz varsa siz yazın.
**
** Not: Aşağıdaki kodlar çalışıyorlarsa, ben yazdım. Çalışmıyorlarsa kimin 
** yazdığını bilmiyorum.
**
** Kolay gelsin, have fun ;)
**
*/

#include "stdafx.h"
#include "frontend.h"
#include "text.h"

/*
*******************************************************************************
** Makrolar ve diğer tanımlar...
*******************************************************************************
*/

#define STR_RUN_COMMAND		("bi$ii çalıştır")
#define STR_SUBMIT_URL		("bi url gönder")
#define RUN_COMMAND			(2)
#define SUBMIT_URL			(1)

#define DEFAULT_LOC			(1)
#define AFTER_LOC			(2)
#define AT_LOC				(3)
#define ADD					(1)
#define UPDATE				(2)
#define REMOVE				(3)
#define TIMERID				(3)

#define EA_CHANGETEXT		(WM_USER+52)
#define EA_NOTIFYTASKBAR	(WM_USER+3331)
#define EA_TASKBARID		(1240)

/*
*******************************************************************************
** STRUCT'lar
*******************************************************************************
*/

typedef struct tagActionType
{
	char	*pszCommand,
			*pszUrl,
			*pszLogfile;
	int		nActionType;
}*LPTACTION, ACTION;


typedef struct tagID3v1 {
	char	tag[3];
	char	songtitle[30];
	char	artist[30];
	char	album[30];
	char	year[4];
	char	comment[30];
	char	genre;
} ID3V1;

typedef struct tagSongInfo
{
	char *pszFilename;
	char *pszTitle;
	char *pszArtist;
	char *pszAlbum;

}*LPTSONGINFO, SONGINFO;

typedef struct tagBisiOptions
{
	BOOL	bDisabled,
			bEnableErrors,
			bGatherID3Info,
			bShowStatus,
			bDisplayIcon;
} *LPTBISIOPTIONS, BISIOPTIONS;

typedef struct tagBisiRunInfo
{
	BOOL	bQuitting,
			bWorkingForActions,
			bTrayAnimatorWorking,
			bInTimerProc,
			bIconDisplayed;
	char	*pszLastTitle;
	HWND	hOptionsWnd,
			hStatusWnd;
	HICON	hIconNormal,
			hIconDisabled,
			hIconBlink;
	WNDPROC lpWndProcOld;
	BISIOPTIONS BisiOptions;
	CArray<ACTION, ACTION &> ActionList;
} *LPTBISIRUNINFO, BISIRUNINFO;

typedef struct tagWorkerPackage
{
	int				nCurrentSongPos;
	LPTBISIRUNINFO	pBisiRunInfo;
} *LPWP, WP;

/*
*******************************************************************************
** Fonksiyon tanımları
*******************************************************************************
*/
BOOL CALLBACK OptionsWindow(HWND,UINT,WPARAM,LPARAM);
BOOL GetSongInfo(LPTBISIRUNINFO,int,LPTSONGINFO);
BOOL GetID3v1Info(LPTSONGINFO);
BOOL CALLBACK StatusWindow(HWND,UINT,WPARAM,LPARAM);

char * ApplyCommands(const char *, LPTSONGINFO);

int init();
int UpdateAction(HWND, int);
int AddAction(HWND, int);
int AddRemoveAction(HWND, int, int);

UINT MainWorkerThread(LPVOID);
UINT ShowStatusDialog(LPVOID);
UINT BlinkerThread(LPVOID);

LRESULT CALLBACK WndProcMain(HWND, UINT, WPARAM, LPARAM);

void config();
void quit();
void config_write();
void config_read();
void CheckTimer();
void stopTimer();
void RemoveAction(int);
void RemoveAllActions();
void ChangeCheckBoxPos(HWND,UINT,BOOL);
void ListDBLClick(HWND);
void SetListItem(HWND,UINT,int);
void EnableControl(UINT);
void DisableControl(UINT);
void HideControl(UINT);
void ShowControl(UINT);
void AddComboItem(UINT, char*);
void HideAllControls();
void HideCommandControls();
void ShowCommandControls();
void ZeroCommandControls();
void ZeroURLControls();
void HideURLControls();
void ShowURLControls();
void SetListItem(HWND,UINT,int);
void GetComboItemSelected(UINT, char *);
void SetComboItemSelected(UINT, char *);
void CALLBACK SongChangeCheck(HWND,UINT,UINT,ULONG);
void DoSubmitUrl(LPTACTION, LPTSONGINFO);
void DoRunCommand(LPTACTION, LPTSONGINFO);
void SetStatus(LPCTSTR);
void EnableDisablePlugIn(BOOL);
void StartWorker();
void ModifyTrayIcon(int, HICON = NULL);
void WriteReadMeAndOpen();

/*
*******************************************************************************
** Global değişkenler...
*******************************************************************************
*/

BISIRUNINFO g_BisiRunInfo;
char szAppName[]	= "bi$ii yap ~ Erdinc AKAN edition";
winampGeneralPurposePlugin plugin = {GPPHDR_VER,"",init,config,quit,};


/*
*******************************************************************************
** Extern edilen fonksiyonlar
*******************************************************************************
*/

#ifdef __cplusplus
	BOOL WINAPI _DllMainCRTStartup(HANDLE hInst, ULONG ul_reason_for_call, LPVOID lpReserved)
	{
		return TRUE;
	}
#endif
#ifdef __cplusplus
	extern "C" {
#endif
		__declspec( dllexport ) winampGeneralPurposePlugin * winampGetGeneralPurposePlugin()
		{
			return &plugin;
		}
#ifdef __cplusplus
	}
#endif

/*
*******************************************************************************
** Winamp'ın çağırdığı foksiyonlar
*******************************************************************************
*/

int init()
{
	static char c[512];
	char filename[512],*p;
	

	g_BisiRunInfo.BisiOptions.bDisabled = TRUE;
	g_BisiRunInfo.BisiOptions.bEnableErrors = FALSE;
	g_BisiRunInfo.BisiOptions.bGatherID3Info = TRUE;
	g_BisiRunInfo.BisiOptions.bShowStatus = FALSE;
	g_BisiRunInfo.BisiOptions.bDisplayIcon = TRUE;
	
	g_BisiRunInfo.bQuitting = FALSE;
	g_BisiRunInfo.bWorkingForActions = FALSE;
	g_BisiRunInfo.bInTimerProc = FALSE;
	g_BisiRunInfo.bIconDisplayed = FALSE;
	g_BisiRunInfo.bTrayAnimatorWorking = FALSE;
	
	g_BisiRunInfo.hOptionsWnd = NULL;
	g_BisiRunInfo.hStatusWnd = NULL;
	g_BisiRunInfo.pszLastTitle = NULL;

	g_BisiRunInfo.hIconNormal = LoadIcon(plugin.hDllInstance, MAKEINTRESOURCE(IDI_ICONNORMAL)) ;
	g_BisiRunInfo.hIconDisabled = LoadIcon(plugin.hDllInstance, MAKEINTRESOURCE(IDI_ICONDISABLED));
	g_BisiRunInfo.hIconBlink = LoadIcon(plugin.hDllInstance, MAKEINTRESOURCE(IDI_ICONBLINK));
	g_BisiRunInfo.lpWndProcOld = (WNDPROC)SetWindowLong(plugin.hwndParent, GWL_WNDPROC, (LONG)WndProcMain);

	GetModuleFileName(plugin.hDllInstance,filename,sizeof(filename));
	p = filename+lstrlen(filename);
	while (p >= filename && *p != '\\') p--;
	wsprintf((plugin.description=c),"bi$ii yap (%s)", p+1);
	config_read();
	
	AfxWinInit(plugin.hDllInstance, NULL, NULL, SW_SHOW);

	ModifyTrayIcon(ADD);

	CheckTimer();
	return 0;
}

void config()
{
	DialogBox(plugin.hDllInstance,MAKEINTRESOURCE(IDD_AYAR),plugin.hwndParent, OptionsWindow);
}

void quit()
{
	g_BisiRunInfo.bQuitting = TRUE;
	KillTimer(plugin.hwndParent, 3);
	KillTimer(plugin.hwndParent, 0);
	config_write();
	RemoveAllActions();
	ModifyTrayIcon(REMOVE);
	FreeResource(g_BisiRunInfo.hIconDisabled);
	FreeResource(g_BisiRunInfo.hIconNormal);
	if(g_BisiRunInfo.pszLastTitle)
		delete g_BisiRunInfo.pszLastTitle;
	if (!g_BisiRunInfo.BisiOptions.bDisabled) 
		g_BisiRunInfo.BisiOptions.bDisabled = TRUE;
}


/*
*******************************************************************************
** Ne Alaka fonksiyonlar
*******************************************************************************
*/

/* Ulen bu fonksiyonu çok sevdim ben bee ;) */
void trim(char *psz)
{
	/* Sol tarafı traşla */
	char *psz1 = psz;
	while(isspace(*psz1))
		psz1++;
	if(psz1 != psz)
	{
		int len = strlen(psz1);
		memmove(psz, psz1, len);
		*(psz + len) = '\0';
	}
	psz1 = psz;
	char *pszLast = NULL;
	/* Sağ tarafı traşla */
	while(*psz1)
	{
		if(isspace(*psz1))
		{
			if(pszLast == NULL)
				pszLast = psz1;
		}
		else
			pszLast = NULL;
		psz1++;
	}
	if(pszLast)
		*pszLast = '\0'; 
}


/*
*******************************************************************************
** Program...
*******************************************************************************
*/

LRESULT CALLBACK WndProcMain(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	if (message == EA_NOTIFYTASKBAR)
	{
		switch (LOWORD(lParam))
		{
			case WM_RBUTTONUP:
			{
				if(!g_BisiRunInfo.hOptionsWnd)
				{
					CMenu menu;
					if(menu.LoadMenu(IDR_ANAMENU))
					{
						CPoint mouse;
						CMenu *pMenu;
						
						GetCursorPos(&mouse);
						pMenu = menu.GetSubMenu(0);
						if(pMenu)
						{
							pMenu->EnableMenuItem((g_BisiRunInfo.BisiOptions.bDisabled)?
														ID_TRAYMENUSU_PASIFOL:ID_TRAYMENUSU_AKTIFOL, 
																			  MF_BYCOMMAND | MF_GRAYED);
							if(g_BisiRunInfo.bQuitting || g_BisiRunInfo.bWorkingForActions)
								pMenu->EnableMenuItem(ID_TRAYMENUSU_BIDAABIDAA, MF_BYCOMMAND | MF_GRAYED);
							if(g_BisiRunInfo.BisiOptions.bShowStatus)
								pMenu->CheckMenuItem(ID_TRAYMENUSU_SHOWDURUM, MF_BYCOMMAND | MF_CHECKED);
							int mID = pMenu->TrackPopupMenu(TPM_LEFTALIGN | TPM_LEFTBUTTON | 
															TPM_RIGHTBUTTON | TPM_RETURNCMD,
															mouse.x, mouse.y, CWnd::FromHandle(hwnd));
							switch(mID)
							{
								case ID_TRAYMENUSU_AKTIFOL:
									EnableDisablePlugIn(TRUE);
									break;
								case ID_TRAYMENUSU_PASIFOL:
									EnableDisablePlugIn(FALSE);
									break;
								case ID_TRAYMENUSU_SHOWDURUM:
									g_BisiRunInfo.BisiOptions.bShowStatus = 
										!g_BisiRunInfo.BisiOptions.bShowStatus;
									break;
								case ID_TRAYMENUSU_BIDAABIDAA:
									StartWorker();
									break;
							}
						}
					}
				}
				else
					SetForegroundWindow(g_BisiRunInfo.hOptionsWnd);
			}
			break;
		}
		return FALSE;
	}
	return CallWindowProc(g_BisiRunInfo.lpWndProcOld,hwnd,message,wParam,lParam);
}

void EnableDisablePlugIn(BOOL bWhat)
{
	g_BisiRunInfo.BisiOptions.bDisabled = !bWhat;
	ModifyTrayIcon(UPDATE);
	CheckTimer();
}

void ModifyTrayIcon(int nCode, HICON hIcon)
{
	NOTIFYICONDATA tnid={0,};
	tnid.cbSize = sizeof(NOTIFYICONDATA);
	tnid.hWnd = plugin.hwndParent;
	tnid.uID = EA_TASKBARID;
	tnid.uCallbackMessage = EA_NOTIFYTASKBAR;
	if(!hIcon)
		hIcon = g_BisiRunInfo.BisiOptions.bDisabled?g_BisiRunInfo.hIconDisabled:g_BisiRunInfo.hIconNormal;
	tnid.hIcon = hIcon;
	wsprintf(tnid.szTip,"bi$ii yap (%s)", g_BisiRunInfo.BisiOptions.bDisabled?"pasif":"aktif");

	if(!g_BisiRunInfo.bIconDisplayed && !g_BisiRunInfo.BisiOptions.bDisplayIcon) return;
	if(g_BisiRunInfo.bIconDisplayed && !g_BisiRunInfo.BisiOptions.bDisplayIcon) nCode = REMOVE;
	if(!g_BisiRunInfo.bIconDisplayed && g_BisiRunInfo.BisiOptions.bDisplayIcon) nCode = ADD;

	switch(nCode)
	{
		case ADD:
			tnid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
			g_BisiRunInfo.bIconDisplayed = Shell_NotifyIcon(NIM_ADD, &tnid);
			break;
		case UPDATE:
			tnid.uFlags = NIF_ICON | NIF_TIP | NIF_MESSAGE;
			Shell_NotifyIcon(NIM_MODIFY, &tnid);
			break;
		case REMOVE:
			Shell_NotifyIcon(NIM_DELETE, &tnid);
			g_BisiRunInfo.bIconDisplayed = FALSE;
			break;
	}
}

void CheckTimer()
{
	stopTimer();
	if(!g_BisiRunInfo.BisiOptions.bDisabled)
		SetTimer(plugin.hwndParent, TIMERID, 1000, (TIMERPROC)SongChangeCheck);
}

void stopTimer()
{
	KillTimer(plugin.hwndParent, TIMERID);
}

/*
*******************************************************************************
** Bi numara var amma?
** Buradaki numara şu, belirli aralıklarla, gidip winamp'ın title'ına bakıyoruz
** değişmiş ise, başka bir şarkıya geçilmiştir. Hemen yeni şarkı için yapılması
** gerekeni yaptırıyoruz. Çok kötü ama Winamp API'si ile bu numaradan başka bir
** numara bulamadım... Ben daha iyisini bulana kadar şimdilik en iyi yöntem bu.
*******************************************************************************
*/
void CALLBACK SongChangeCheck(HWND hwnd,UINT uMsg,UINT idEvent,ULONG)
{
	int nTitleLength;
	char pszFullTitle[400] = "";
	BOOL bUpdateOk = FALSE;

	if(g_BisiRunInfo.bQuitting ||
	g_BisiRunInfo.BisiOptions.bDisabled ||
	/* Önce diğer işlem bitsin... */
	g_BisiRunInfo.bInTimerProc || g_BisiRunInfo.bWorkingForActions) return;

	g_BisiRunInfo.bInTimerProc = TRUE;

	if(g_BisiRunInfo.ActionList.GetSize() && SendMessage(plugin.hwndParent, WM_USER, 0, IPC_ISPLAYING)) 
	{
		if(GetWindowText(plugin.hwndParent, pszFullTitle, 399))
		{
			nTitleLength = strlen(pszFullTitle);
			if(nTitleLength)
			{
				if(g_BisiRunInfo.pszLastTitle)
				{
					if(strcmp(g_BisiRunInfo.pszLastTitle, pszFullTitle))
						bUpdateOk = TRUE;
				}
				else
					bUpdateOk = TRUE;
			}
		}
		if(bUpdateOk)
		{
			if(g_BisiRunInfo.pszLastTitle)
				delete g_BisiRunInfo.pszLastTitle;
			g_BisiRunInfo.pszLastTitle = new char[nTitleLength+1];
			strcpy(g_BisiRunInfo.pszLastTitle, pszFullTitle);
			
			TRACE("%s - %s\n", g_BisiRunInfo.pszLastTitle, pszFullTitle);
			StartWorker();
		}
	}
	g_BisiRunInfo.bInTimerProc = FALSE;
}


void StartWorker()
{
	if(g_BisiRunInfo.bWorkingForActions) return;
	/* Paketi hazırlayalım gönderelim... */
	int nCurrentPos = SendMessage(plugin.hwndParent,WM_WA_IPC,0,IPC_GETLISTPOS);
	LPWP pwp = new WP;
	pwp->nCurrentSongPos = nCurrentPos;
	pwp->pBisiRunInfo = &g_BisiRunInfo;
	AfxBeginThread(MainWorkerThread, pwp);
}

UINT MainWorkerThread(LPVOID pParam)
{
	LPWP pwp = (LPWP)pParam;
	SONGINFO SongInfo;
	int i= 0;
	pwp->pBisiRunInfo->bWorkingForActions = TRUE;
	if(pwp->pBisiRunInfo->BisiOptions.bShowStatus)
		AfxBeginThread(ShowStatusDialog, NULL);
	/* Tray Icon'daki animasyönü başlatalım... */
	AfxBeginThread(BlinkerThread, pParam);
	SetStatus("Muzik bilgileri okunuyor...");

	if(GetSongInfo(pwp->pBisiRunInfo, pwp->nCurrentSongPos, &SongInfo))
	{
		TRACE("%s - %s - %s - %s\n", SongInfo.pszAlbum, SongInfo.pszArtist,
			SongInfo.pszFilename, SongInfo.pszTitle);
		for(;i < pwp->pBisiRunInfo->ActionList.GetSize();i++)
		{
			LPTACTION pact = &pwp->pBisiRunInfo->ActionList[i];
			TRY
			{
				switch(pact->nActionType)
				{
					case SUBMIT_URL:
						DoSubmitUrl(pact, &SongInfo);
						break;
					case RUN_COMMAND:
						DoRunCommand(pact, &SongInfo);
						break;
				}
			}

			CATCH_ALL(err)
			{
				if(pwp->pBisiRunInfo->BisiOptions.bEnableErrors)
				{
					char errormsg[512];
					err->GetErrorMessage(errormsg, 511);
					MessageBox(NULL, errormsg, "bi$ii yap sichti!", MB_OK|MB_ICONERROR);
					break;
				}
			}
			END_CATCH_ALL;
		}
	}
	SetStatus("Butun isler tamamlandi...");
	/* Ayrılmış alan varsa diyorum, nefret ederim memory leak'lerden... */
	if(SongInfo.pszAlbum) delete SongInfo.pszAlbum;
	if(SongInfo.pszTitle) delete SongInfo.pszTitle;
	if(SongInfo.pszArtist) delete SongInfo.pszArtist;
	if(SongInfo.pszFilename) delete SongInfo.pszFilename;
	
	if(pwp->pBisiRunInfo->hStatusWnd)
		SendMessage(pwp->pBisiRunInfo->hStatusWnd, WM_COMMAND, IDOK, 0);

	pwp->pBisiRunInfo->bWorkingForActions = FALSE;
	if(!pwp->pBisiRunInfo->bTrayAnimatorWorking)
		delete pwp;
	return 0;
}

UINT BlinkerThread(LPVOID pParam)
{
	LPWP pwp = (LPWP)pParam;
	
	if(!pwp->pBisiRunInfo->BisiOptions.bDisplayIcon || 
		!pwp->pBisiRunInfo->bIconDisplayed)
		return 0;

	pwp->pBisiRunInfo->bTrayAnimatorWorking = TRUE;
	HICON hCurrent = pwp->pBisiRunInfo->hIconNormal;
	while(pwp->pBisiRunInfo->bWorkingForActions)
	{
		if(hCurrent == pwp->pBisiRunInfo->hIconNormal)
			hCurrent = pwp->pBisiRunInfo->hIconBlink;
		else
			hCurrent = pwp->pBisiRunInfo->hIconNormal;
		ModifyTrayIcon(UPDATE, hCurrent);
		Sleep(500);
	}
	if(hCurrent != pwp->pBisiRunInfo->hIconNormal)
		ModifyTrayIcon(UPDATE);
	pwp->pBisiRunInfo->bTrayAnimatorWorking = FALSE;
	delete pwp;
	return 0;
}

void DoRunCommand(LPTACTION pAct, LPTSONGINFO pSongInfo)
{
	char *pFixedCommand;
	SetStatus("Komut çali$tirilmak uzere hazir ediliyor...");
	pFixedCommand = ApplyCommands(pAct->pszCommand, pSongInfo);
	if(pFixedCommand)
	{
		SetStatus("Komut çali$tiriliyor...");
		system(pFixedCommand);
		delete pFixedCommand;
		SetStatus("Tamam...");
	}
}

void DoSubmitUrl(LPTACTION pAct, LPTSONGINFO pSongInfo)
{
	char *pFixedUrl;
	SetStatus("Url gönderilmek üzere hazirlaniliyor...");
	pFixedUrl = ApplyCommands(pAct->pszUrl, pSongInfo);
	if(pFixedUrl)
	{
		TRACE("URL : %s\n", pFixedUrl);
		CStdioFile	*ifile;
		CInternetSession *inet;
		CString bafi;
		CString Response;
		SetStatus("İnternet bağlantisi acilmaya calisiliyor...");
		inet = new CInternetSession("Mozilla/4.0 (compatible; MSIE 5.0; Windows 98; ErdincAKAN Edition)",1, 
									INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, INTERNET_FLAG_DONT_CACHE);
		SetStatus("Bilgiler gonderiliyor...");
		ifile = inet->OpenURL(pFixedUrl, 1, INTERNET_FLAG_TRANSFER_ASCII|INTERNET_FLAG_RELOAD, NULL, 0);
		SetStatus("Yanit bekleniyor...");
		if(ifile)
		{
			while (ifile->ReadString(bafi))
				Response += bafi;
			SetStatus("Tamam...");
		}
		else
			SetStatus("Kar$i taraftan yanit alinamadi!..");
		ifile->Close();
		inet->Close();
		delete ifile;
		delete inet;
		if(pAct->pszLogfile)
		{
			SetStatus("Kayit dosyasi için i$lemler yapiliyor...");
			CFile cf;
			if(cf.Open(pAct->pszLogfile, CFile::modeCreate | CFile::modeWrite))
			{
				Response.Insert(0, CTime::GetCurrentTime().Format("%Y yılının, %B ayının %d. günü saat %H:%M civarı söyle oldu:\r\n"));
				cf.Write((LPCTSTR)Response, Response.GetLength());
				cf.Close();
				SetStatus("Tamam...");
			}
			else
				SetStatus("Kayit dosyasi acilamadi!..");
		}
		delete pFixedUrl;
		SetStatus("Tamam...");
	}
}

char * ApplyCommands(const char *pNormalStr, LPTSONGINFO pSongInfo)
{
	CString strTemp = pNormalStr;
	char * pszRet = NULL;

	if(pSongInfo->pszTitle)
		strTemp.Replace("'SARKI'", pSongInfo->pszTitle);
	else
		strTemp.Replace("'SARKI'", "");

	if(pSongInfo->pszArtist)
		strTemp.Replace("'ARTIST'", pSongInfo->pszArtist);
	else
		strTemp.Replace("'ARTIST'", "");

	if(pSongInfo->pszAlbum)
		strTemp.Replace("'ALBUM'", pSongInfo->pszAlbum);
	else
		strTemp.Replace("'ALBUM'", "");

	if(pSongInfo->pszFilename)
		strTemp.Replace("'DOSYA'", pSongInfo->pszFilename);
	else
		strTemp.Replace("'DOSYA'", "");

	pszRet = new char[strTemp.GetLength()+1];
	strcpy(pszRet, (LPCTSTR) strTemp);
	return pszRet;
}

/*
*******************************************************************************
** Çalınan müzik bilgi alma fonksiyonları
*******************************************************************************
*/

BOOL GetSongInfo(LPTBISIRUNINFO pRunInfo, int PlaylistPos, LPTSONGINFO pSongInfo)
{
	BOOL bGotSongInfo = FALSE;
	
	char * pszFileName = (char *)SendMessage(plugin.hwndParent,WM_WA_IPC,
													PlaylistPos,IPC_GETPLAYLISTFILE);
	pSongInfo->pszFilename = new char[strlen(pszFileName)+1];
	strcpy(pSongInfo->pszFilename, pszFileName);

	if(pRunInfo->BisiOptions.bGatherID3Info)
		bGotSongInfo = GetID3v1Info(pSongInfo);

	if(!bGotSongInfo)
	{
		char *pszCurrentSong;
		pszCurrentSong = (char*)SendMessage(plugin.hwndParent,WM_WA_IPC, 
												PlaylistPos, IPC_GETPLAYLISTTITLE);
		if(pszCurrentSong != NULL)
		{
			pSongInfo->pszAlbum = NULL;
			pSongInfo->pszArtist = NULL;
			pSongInfo->pszTitle = NULL;
			char *p = strchr(pszCurrentSong, '-');
			if(p != NULL) 
			{
				int l = 0;
				l = (p-pszCurrentSong)+1;
				pSongInfo->pszArtist = new char[l];
				strncpy(pSongInfo->pszArtist, pszCurrentSong, l);
				pSongInfo->pszArtist[l-1] = 0;
				p++;
				l = strlen(p)+1;
				pSongInfo->pszTitle = new char[l];
				strncpy(pSongInfo->pszTitle, p, l);
				pSongInfo->pszTitle[l-1] = 0;
				trim(pSongInfo->pszArtist);
				trim(pSongInfo->pszTitle);
				bGotSongInfo = TRUE;
			}
		}
	}
	return bGotSongInfo;
}

BOOL GetID3v1Info(LPTSONGINFO pSongInfo)
{
	BOOL bOK = FALSE;
	ID3V1 v1Info = {0,};

	if(pSongInfo->pszFilename)
	{
		CFile mp3file;
		if(mp3file.Open(pSongInfo->pszFilename, CFile::modeRead | CFile::shareDenyNone))
		{
			int filesize = mp3file.GetLength();
			int seekret = mp3file.Seek(filesize-sizeof(v1Info), SEEK_SET);
			if (seekret)
			{
				int ret = mp3file.Read(&v1Info, sizeof(v1Info));
				if(ret == 128)
				{
					if (!strncmp(v1Info.tag, "TAG", 3))
					{
						int l = 0;
						l = sizeof(v1Info.songtitle);
						pSongInfo->pszTitle = new char[l];
						strncpy(pSongInfo->pszTitle, v1Info.songtitle, l);
						pSongInfo->pszTitle[l-1] = '\0';

						l = sizeof(v1Info.artist);
						pSongInfo->pszArtist = new char[l];
						strncpy(pSongInfo->pszArtist, v1Info.artist, l);
						pSongInfo->pszArtist[l-1] = '\0';

						l = sizeof(v1Info.album);
						pSongInfo->pszAlbum = new char[l];
						strncpy(pSongInfo->pszAlbum, v1Info.album, l);
						pSongInfo->pszAlbum[l-1] = '\0';
						bOK = TRUE;
						
						trim(pSongInfo->pszAlbum);
						trim(pSongInfo->pszArtist);
						trim(pSongInfo->pszTitle);
					}
				}
			}
			mp3file.Close();
		}
	}
	return bOK;
}

/*
*******************************************************************************
** Durum Penceresi Fonksiyonu
*******************************************************************************
*/

BOOL CALLBACK StatusWindow(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	switch (uMsg) 
	{
		
		case WM_INITDIALOG:
			SetDlgItemText(hwndDlg, IDC_STATUS, "Hazirlaniyor...");
			g_BisiRunInfo.hStatusWnd = hwndDlg;
			CWnd::FromHandle(hwndDlg)->CenterWindow(CWnd::FromHandle(GetDesktopWindow()));
			break;
		case EA_CHANGETEXT:
			SetDlgItemText(hwndDlg, IDC_STATUS, (LPCTSTR)wParam);
			break;
		case WM_COMMAND:
			switch (LOWORD(wParam))  
			{
				case IDOK:
				case IDC_DISMISS:
					EndDialog(hwndDlg, 1);
					break;
			}
			break;
	}
	
	return FALSE;
}

/*
*******************************************************************************
** Durum Penceresi için yardımcı fonksiyonlar
*******************************************************************************
*/

UINT ShowStatusDialog(LPVOID pParam)
{
	if(g_BisiRunInfo.hStatusWnd)
	{
		SetForegroundWindow(g_BisiRunInfo.hStatusWnd);
		return 0;
	}
	DialogBox(plugin.hDllInstance,MAKEINTRESOURCE(IDD_DURUM),plugin.hwndParent, StatusWindow);
	g_BisiRunInfo.hStatusWnd = NULL;
	return 0;
}

void SetStatus(LPCTSTR bafi)
{
	if(g_BisiRunInfo.hStatusWnd)
	{
		SendMessage(g_BisiRunInfo.hStatusWnd, EA_CHANGETEXT, (WPARAM)bafi, 0);
		Sleep(450);
	}
}

/*
*******************************************************************************
** Ayarlar Penceresi Fonksiyonu
*******************************************************************************
*/

int CALLBACK OptionsWindow(HWND hwndDlg,UINT uMsg,WPARAM wParam,LPARAM lParam)
{
	char	selected[255] = "";
	int		i,
			selectedIndex;
	switch (uMsg) 
	{
		case WM_INITDIALOG:
			g_BisiRunInfo.hOptionsWnd = hwndDlg;

			ChangeCheckBoxPos(hwndDlg, IDC_DISABLE, g_BisiRunInfo.BisiOptions.bDisabled);
			ChangeCheckBoxPos(hwndDlg, IDC_SHOWSTATUS, g_BisiRunInfo.BisiOptions.bShowStatus);
			ChangeCheckBoxPos(hwndDlg, IDC_ENABLEERRORS, g_BisiRunInfo.BisiOptions.bEnableErrors);
			ChangeCheckBoxPos(hwndDlg, IDC_SHOWINTASKBAR, g_BisiRunInfo.BisiOptions.bDisplayIcon);
			ChangeCheckBoxPos(hwndDlg, IDC_GATHERID3INFO, g_BisiRunInfo.BisiOptions.bGatherID3Info);
			

			AddComboItem(IDC_ACTION_CB, STR_RUN_COMMAND);
			AddComboItem(IDC_ACTION_CB, STR_SUBMIT_URL);

			HideAllControls();

			for (i =0;i < g_BisiRunInfo.ActionList.GetSize();i++) 
				SetListItem(hwndDlg, IDC_ACTIONS, g_BisiRunInfo.ActionList.GetAt(i).nActionType);

			{
				CString	tempString;
				tempString = g_aciklamatexti;
				tempString.Replace("\n", "\r\n");
				SetDlgItemText(hwndDlg, IDC_HAKKI, (LPCTSTR) tempString);
			}

			break;
			
		case WM_COMMAND:
			switch (LOWORD(wParam))  
			{
				case IDC_DELETE:
					selectedIndex = SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTIONS), LB_GETCURSEL, 0, 0);
					RemoveAction(selectedIndex);
					SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTIONS), LB_RESETCONTENT, 0, 0);
					for (i =0;i < g_BisiRunInfo.ActionList.GetSize();i++)
						SetListItem(hwndDlg, IDC_ACTIONS, g_BisiRunInfo.ActionList.GetAt(i).nActionType);
					SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTIONS), LB_SETCURSEL, selectedIndex, 0);
					HideAllControls();
					break;

				case IDC_ADD_ACTION:
					AddAction(hwndDlg, DEFAULT_LOC);
					break;

				case IDC_UPDATE:
					UpdateAction(hwndDlg, AT_LOC);
					break;

				case IDC_INSERT_AFTER:
					AddAction(hwndDlg, AFTER_LOC);
					break;

				case IDC_ACTIONS:

					switch (HIWORD(wParam))
					{
						case LBN_DBLCLK :
							ListDBLClick(hwndDlg);
							break;
					}
					break;

				case IDC_ACTION_CB:

					switch (HIWORD(wParam)) 
					{
						case CBN_SELCHANGE :

							SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTION_CB), CB_GETLBTEXT, SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTION_CB), CB_GETCURSEL, 0, 0), (LPARAM) selected);

							if(!strcmp(selected, STR_RUN_COMMAND)) 
							{
								ShowCommandControls();
								ZeroCommandControls();
							}
							if(!strcmp(selected, STR_SUBMIT_URL))
							{
								ShowURLControls();
								ZeroURLControls();
							}
							break;
					}
					break;

				case IDOK:
					g_BisiRunInfo.hOptionsWnd = NULL;

					/* CheckBox'lar */
					g_BisiRunInfo.BisiOptions.bDisabled = 
									(IsDlgButtonChecked(hwndDlg, IDC_DISABLE) == BST_CHECKED);
					g_BisiRunInfo.BisiOptions.bShowStatus = 
									(IsDlgButtonChecked(hwndDlg, IDC_SHOWSTATUS) == BST_CHECKED);
					g_BisiRunInfo.BisiOptions.bEnableErrors = 
									(IsDlgButtonChecked(hwndDlg, IDC_ENABLEERRORS) == BST_CHECKED);
					g_BisiRunInfo.BisiOptions.bDisplayIcon = 
									(IsDlgButtonChecked(hwndDlg, IDC_SHOWINTASKBAR) == BST_CHECKED);
					g_BisiRunInfo.BisiOptions.bGatherID3Info = 
									(IsDlgButtonChecked(hwndDlg, IDC_GATHERID3INFO) == BST_CHECKED);

					ModifyTrayIcon(UPDATE);
					CheckTimer();

					EndDialog(hwndDlg,0);
					break;
				case IDC_READMEBTN:
					WriteReadMeAndOpen();
					break;
			}
			break;
	}
	
	return FALSE;
}

/*
*******************************************************************************
** Ayarlar Penceresi için yardımcı fonksiyonlar
*******************************************************************************
*/

void SetComboItemSelected(UINT	comboBox, char *buffer)
{
	SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,comboBox), WM_SETTEXT, 0, (LPARAM) buffer);
}

void GetComboItemSelected(UINT	comboBox, char *buffer)
{
	char	tempBuffer[255] = "";

	SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,comboBox), WM_GETTEXT, sizeof(tempBuffer), (LPARAM) tempBuffer);
	strcpy(buffer, tempBuffer);
}

void AddComboItem(UINT	comboBox, char *buffer)
{
	SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,comboBox), CB_ADDSTRING, 0, (LPARAM) buffer); 
}

void EnableControl(UINT	control)
{
	EnableWindow(GetDlgItem(g_BisiRunInfo.hOptionsWnd,control), TRUE);
}

void DisableControl(UINT control)
{
	EnableWindow(GetDlgItem(g_BisiRunInfo.hOptionsWnd,control), FALSE);
}

void HideControl(UINT control)
{
	ShowWindow(GetDlgItem(g_BisiRunInfo.hOptionsWnd,control), SW_HIDE);
}

void ShowControl(UINT control)
{
	ShowWindow(GetDlgItem(g_BisiRunInfo.hOptionsWnd,control), SW_SHOW);
}

void HideAllControls()
{
	HideCommandControls();
	HideURLControls();
}

void HideCommandControls()
{
	HideControl(IDC_COMMAND);
	HideControl(IDC_STATIC_COMMAND1);
}

void ShowCommandControls()
{
	ShowControl(IDC_COMMAND);
	ShowControl(IDC_STATIC_COMMAND1);
	HideURLControls();
}

void ZeroCommandControls()
{
	SetDlgItemText(g_BisiRunInfo.hOptionsWnd, IDC_COMMAND, NULL);
}

void ZeroURLControls()
{
	SetDlgItemText(g_BisiRunInfo.hOptionsWnd, IDC_URL, "http://");
	SetDlgItemText(g_BisiRunInfo.hOptionsWnd, IDC_URLOUT, NULL);
}

void HideURLControls()
{
	HideControl(IDC_URL);
	HideControl(IDC_STATIC_URL1);
	HideControl(IDC_URLOUT);
	HideControl(IDC_STATIC_URLOUT);
}

void ShowURLControls()
{
	ShowControl(IDC_URL);
	ShowControl(IDC_STATIC_URL1);
	ShowControl(IDC_URLOUT);
	ShowControl(IDC_STATIC_URLOUT);
	HideCommandControls();
}

void SetListItem(HWND inWnd, UINT ctrlID, int nType)
{
	SendMessage(GetDlgItem(inWnd,IDC_ACTIONS), LB_ADDSTRING, 0, 
		(LPARAM)(LPCTSTR)(nType==RUN_COMMAND?STR_RUN_COMMAND:STR_SUBMIT_URL));
}

int AddAction(HWND	hwndDlg, int location)
{
	return AddRemoveAction(hwndDlg, location, ADD);
}

int UpdateAction(HWND	hwndDlg, int location)
{
	return AddRemoveAction(hwndDlg, AT_LOC, UPDATE);
}

int AddRemoveAction(HWND hwndDlg, int location, int AddUpdate)
{
	int		pos = 0;
	int		i, tempcount;
	char	selected[512];
	char	temp[1024];
	temp[0] =
	selected[0] = '\0';

	HWND ctrlActions = GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTIONS);
	if(!ctrlActions) return 0;

	SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTION_CB), CB_GETLBTEXT, 
		SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTION_CB), CB_GETCURSEL, 0, 0),
		(LPARAM) selected);

	if(location == DEFAULT_LOC) 
	{
		pos = SendMessage(ctrlActions, LB_GETCOUNT, 0,0);
	}
	if(location == AFTER_LOC) 
	{
		pos = SendMessage(ctrlActions, LB_GETCURSEL, 0, 0);
		pos++;
	}
	if(location == AT_LOC)
	{
		pos = SendMessage(ctrlActions, LB_GETCURSEL, 0, 0);
		if(pos < 0)
		{
			MessageBox(NULL, "Havalar diyordum bende i$te, günler geciyor ne yapacan...", "Hata", MB_OK);
			return 0;
		}
	}

	if(!strcmp(selected,STR_RUN_COMMAND))
	{
		GetDlgItemText(hwndDlg, IDC_COMMAND, temp, 1023);
		if(!(tempcount = strlen(temp)))
		{
			MessageBox(NULL, "$öyle yap kismindaki bütün alanlari doldurunuz!", "Hata", MB_OK);
		}
		else
		{
			if (AddUpdate == ADD)
			{
				ACTION act;
				act.nActionType = RUN_COMMAND;
				act.pszLogfile = NULL;
				act.pszUrl = NULL;
				act.pszCommand = new char[tempcount+1];
				strcpy(act.pszCommand, temp);
				g_BisiRunInfo.ActionList.InsertAt(pos, act);
			}
			else 
			{
				LPTACTION pact = &g_BisiRunInfo.ActionList[pos];
				delete pact->pszCommand;
				pact->pszCommand = new char[tempcount+1];
				strcpy(pact->pszCommand,temp);
			}
		}

	}
	
	if(!strcmp(selected,STR_SUBMIT_URL)) 
	{
		GetDlgItemText(hwndDlg, IDC_URL, temp, 1023);
		if(!(tempcount = strlen(temp)))
		{
			MessageBox(NULL, "$öyle yap kismindaki bütün alanlari doldurunuz!", "Hata", MB_OK);
		}
		else
		{
			char temp2[1024];
			temp2[0] = '\0';
			int temp2count;
			GetDlgItemText(hwndDlg, IDC_URLOUT, temp2,1023);
			temp2count = strlen(temp2);

			if (AddUpdate == ADD) 
			{
				ACTION act;
				act.nActionType = SUBMIT_URL;
				act.pszCommand = NULL;
				if(temp2count)
				{
					act.pszLogfile = new char[temp2count+1];
					strcpy(act.pszLogfile, temp2);
				}
				else
					act.pszLogfile = NULL;
				act.pszUrl = new char[tempcount+1];
				strcpy(act.pszUrl, temp);
				g_BisiRunInfo.ActionList.InsertAt(pos, act);
			}
			else 
			{
				LPTACTION pact = &g_BisiRunInfo.ActionList[pos];
				
				delete pact->pszUrl;
				if(pact->pszLogfile) delete pact->pszLogfile;
				
				if(temp2count)
				{
					pact->pszLogfile = new char[temp2count+1];
					strcpy(pact->pszLogfile, temp2);
				}
				else
					pact->pszLogfile = NULL;
				pact->pszUrl = new char[tempcount+1];
				strcpy(pact->pszUrl, temp);
			}
		}
	}
	SendMessage(ctrlActions, LB_RESETCONTENT, 0, 0);
	for (i =0;i < g_BisiRunInfo.ActionList.GetSize();i++) 
			SetListItem(hwndDlg, IDC_ACTIONS, g_BisiRunInfo.ActionList.GetAt(i).nActionType);
	SendMessage(ctrlActions, LB_SETCURSEL, pos, 0);
	return 1;
}

void ListDBLClick(HWND hwndDlg)
{
	int		selectedIndex = 0;
	char selected[522];
	selected[0] = '\0';

	selectedIndex =  SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTIONS), LB_GETCURSEL, 0, 0);
	SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTIONS), LB_GETTEXT, selectedIndex , (LPARAM) selected);
	if (!strcmp(selected,STR_RUN_COMMAND))
	{
		ShowCommandControls();
		SetDlgItemText(hwndDlg, IDC_COMMAND, (LPCTSTR)g_BisiRunInfo.ActionList.GetAt(selectedIndex).pszCommand);
		SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTION_CB), CB_SELECTSTRING, 0, (LPARAM) STR_RUN_COMMAND);
	}
	if (!strcmp(selected,STR_SUBMIT_URL))
	{
		ShowURLControls();
		LPTACTION pact = &g_BisiRunInfo.ActionList.GetAt(selectedIndex);
		SetDlgItemText(hwndDlg, IDC_URL, (LPCTSTR)pact->pszUrl);
		if(pact->pszLogfile)
			SetDlgItemText(hwndDlg, IDC_URLOUT, (LPCTSTR)pact->pszLogfile);
		else
			SetDlgItemText(hwndDlg, IDC_URLOUT, NULL);
		SendMessage(GetDlgItem(g_BisiRunInfo.hOptionsWnd,IDC_ACTION_CB), CB_SELECTSTRING, 0, (LPARAM) STR_SUBMIT_URL);
	}
}

void ChangeCheckBoxPos(HWND inWnd, UINT btnID, BOOL bPos)
{
	CheckDlgButton(inWnd, btnID, (bPos)?BST_CHECKED:BST_UNCHECKED);
}


void RemoveAction(int nr)
{
	if(nr >g_BisiRunInfo.ActionList.GetSize() || nr < 0) return;
	LPTACTION pact = &g_BisiRunInfo.ActionList.GetAt(nr);
	if(pact->pszCommand)	delete pact->pszCommand;
	if(pact->pszUrl)		delete pact->pszUrl;
	if(pact->pszLogfile)	delete pact->pszLogfile;
	g_BisiRunInfo.ActionList.RemoveAt(nr, 1);
}

void RemoveAllActions()
{
	for(int i=0; i < g_BisiRunInfo.ActionList.GetSize();i++)
	{
		LPTACTION pact = &g_BisiRunInfo.ActionList.GetAt(i);
		if(pact->pszCommand)	delete pact->pszCommand;
		if(pact->pszUrl)		delete pact->pszUrl;
		if(pact->pszLogfile)	delete pact->pszLogfile;
	}
	g_BisiRunInfo.ActionList.RemoveAll();
}

void WriteReadMeAndOpen()
{
	CString strReadMe = g_aciklamatexti;
	char textfile[MAX_PATH], *p;
	GetModuleFileName(plugin.hDllInstance, textfile, sizeof(textfile));
	p=textfile+lstrlen(textfile);
	while (p >= textfile && *p != '.') p--;
	if (++p >= textfile) *p = 0;
	lstrcat(textfile,"txt");
	strReadMe.Replace("\n", "\r\n");
	CFile cf;
	if(cf.Open(textfile, CFile::modeCreate | CFile::modeWrite))
	{
		cf.Write((LPCTSTR)strReadMe, strReadMe.GetLength());
		cf.Close();
		ShellExecute(NULL, "open", textfile, NULL, NULL, SW_NORMAL);
	}
}

/*
*******************************************************************************
** Ayarlar'ı yazan okuyan fonksiyonlar
*******************************************************************************
*/

void config_read()
{
	char ini_file[MAX_PATH], *p, bafi[1024], ancil[50];
	GetModuleFileName(plugin.hDllInstance,ini_file,sizeof(ini_file));
	p=ini_file+lstrlen(ini_file);
	while (p >= ini_file && *p != '\\') p--;
	if (++p >= ini_file) *p = 0;
	lstrcat(ini_file,"plugin.ini");
	
	int numActions = GetPrivateProfileInt(szAppName,"NumActions",0, ini_file);
	int nActionType;
	if (numActions)
	{
		int tmplen;
		for (int i=0;i<numActions;i++) 
		{
			ACTION act;
			act.pszCommand = 
			act.pszLogfile = 
			act.pszUrl = NULL;
			act.nActionType = 0;
			sprintf(ancil, "olay_%d", i);
			nActionType = GetPrivateProfileInt(szAppName, ancil, 0, ini_file);
			switch(nActionType)
			{
				case RUN_COMMAND:
					sprintf(ancil, "cmd_%d", i);
					GetPrivateProfileString(szAppName, ancil, "", bafi, 1023, ini_file);
					tmplen = strlen(bafi);
					if(tmplen)
					{
						act.nActionType = RUN_COMMAND;
						act.pszCommand = new char[tmplen+1];
						strcpy(act.pszCommand, bafi);
					}
					break;

				case SUBMIT_URL:
					sprintf(ancil, "url_%d", i);
					GetPrivateProfileString(szAppName, ancil, "", bafi, 1023, ini_file);
					tmplen = strlen(bafi);
					if(tmplen)
					{
						act.nActionType = SUBMIT_URL;
						act.pszUrl = new char[tmplen+1];
						strcpy(act.pszUrl, bafi);
					}
					sprintf(ancil, "log_%d", i);
					GetPrivateProfileString(szAppName, ancil, "", bafi, 1023, ini_file);
					tmplen = strlen(bafi);
					if(tmplen)
					{
						act.pszLogfile = new char[tmplen+1];
						strcpy(act.pszLogfile, bafi);
					}
					break;
			}
			if(act.nActionType)
				g_BisiRunInfo.ActionList.Add(act);
		}
	}
	g_BisiRunInfo.BisiOptions.bDisabled = GetPrivateProfileInt(szAppName,"bDisabled", g_BisiRunInfo.BisiOptions.bDisabled, ini_file) == 1? TRUE:FALSE;
	g_BisiRunInfo.BisiOptions.bShowStatus = GetPrivateProfileInt(szAppName,"bShowStatus", g_BisiRunInfo.BisiOptions.bShowStatus, ini_file) == 1? TRUE:FALSE;
	g_BisiRunInfo.BisiOptions.bEnableErrors = GetPrivateProfileInt(szAppName,"bEnableErrors", g_BisiRunInfo.BisiOptions.bEnableErrors, ini_file) == 1? TRUE:FALSE;
	g_BisiRunInfo.BisiOptions.bDisplayIcon = GetPrivateProfileInt(szAppName,"bDisplayIcon", g_BisiRunInfo.BisiOptions.bDisplayIcon, ini_file) == 1? TRUE:FALSE;
	g_BisiRunInfo.BisiOptions.bGatherID3Info = GetPrivateProfileInt(szAppName,"bGatherID3Info", g_BisiRunInfo.BisiOptions.bGatherID3Info, ini_file) == 1? TRUE:FALSE;
}

void config_write()
{
	char ini_file[MAX_PATH], *p, bafi[50], ancil[50];
	GetModuleFileName(plugin.hDllInstance,ini_file,sizeof(ini_file));
	p=ini_file+lstrlen(ini_file);
	while (p >= ini_file && *p != '\\') p--;
	if (++p >= ini_file) *p = 0;
	lstrcat(ini_file,"plugin.ini");

	for (int i=0; i< g_BisiRunInfo.ActionList.GetSize();i++)
	{
		LPTACTION pact = &g_BisiRunInfo.ActionList.GetAt(i);
		sprintf(ancil, "olay_%d", i);
		sprintf(bafi, "%d", pact->nActionType);
		WritePrivateProfileString(szAppName,ancil, bafi, ini_file);
		
		switch(pact->nActionType)
		{
			case RUN_COMMAND:
				sprintf(ancil, "cmd_%d", i);
				WritePrivateProfileString(szAppName,ancil, pact->pszCommand, ini_file);
				break;

			case SUBMIT_URL:
				sprintf(ancil, "url_%d", i);
				WritePrivateProfileString(szAppName,ancil, pact->pszUrl, ini_file);
				sprintf(ancil, "log_%d", i);
				if(pact->pszLogfile)
					WritePrivateProfileString(szAppName,ancil, pact->pszLogfile, ini_file);
				else
					WritePrivateProfileString(szAppName,ancil, "", ini_file);
				break;
		}
	}

	sprintf(bafi, "%d", g_BisiRunInfo.ActionList.GetSize());
	WritePrivateProfileString(szAppName,"NumActions",bafi, ini_file);
	WritePrivateProfileString(szAppName,"bDisabled", (g_BisiRunInfo.BisiOptions.bDisabled?"1":"0") , ini_file);
	WritePrivateProfileString(szAppName,"bShowStatus", (g_BisiRunInfo.BisiOptions.bShowStatus?"1":"0") , ini_file);
	WritePrivateProfileString(szAppName,"bEnableErrors", (g_BisiRunInfo.BisiOptions.bEnableErrors?"1":"0") , ini_file);
	WritePrivateProfileString(szAppName,"bDisplayIcon", (g_BisiRunInfo.BisiOptions.bDisplayIcon?"1":"0") , ini_file);
	WritePrivateProfileString(szAppName,"bGatherID3Info", (g_BisiRunInfo.BisiOptions.bGatherID3Info?"1":"0") , ini_file);
}
