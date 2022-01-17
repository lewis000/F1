/*
@file
@brief 基于录音接口和讯飞MSC接口封装一个MIC录音识别的模块

@author		taozhang9
@date		2016/05/27
*/

#define FRAME_LEN	640 
#define	BUFFER_SIZE	4096
#define SAMPLE_RATE_16K     (16000)
#define SAMPLE_RATE_8K      (8000)
#define MAX_GRAMMARID_LEN   (32)
#define MAX_PARAMS_LEN      (1024)

static char *g_result = NULL;
static unsigned int g_buffersize = BUFFER_SIZE;

typedef struct _UserData {
	int     build_fini;  //标识语法构建是否完成
	int     update_fini; //标识更新词典是否完成
	int     errcode; //记录语法构建或更新词典回调错误码
	char    grammar_id[32]; //保存语法构建返回的语法ID
}UserData;

void recognize_mic(const char* session_begin_params);
void on_result(const char *result, char is_last);
void on_speech_begin();
void on_speech_end(int reason);

class SR {
public:

	const char * ASR_RES_PATH = "fo|res/asr/common.jet";  //离线语法识别资源路径
#ifdef _WIN64
	const char * GRM_BUILD_PATH = "res/asr/GrmBuilld_x64";  //构建离线语法识别网络生成数据保存路径
#else
	const char * GRM_BUILD_PATH = "res/asr/GrmBuilld";  //构建离线语法识别网络生成数据保存路径
#endif
	const char * GRM_FILE = "call.bnf"; //构建离线识别语法网络所用的语法文件

	int build_grammar(UserData *udata);
	int run_asr(UserData *udata);

};

enum sr_audsrc
{
	SR_MIC,	/* write data from mic */
	SR_USER	/* write data from user by calling API */
};

#define DEFAULT_INPUT_DEVID     (-1)


#define E_SR_NOACTIVEDEVICE		1
#define E_SR_NOMEM				2
#define E_SR_INVAL				3
#define E_SR_RECORDFAIL			4
#define E_SR_ALREADY			5


struct speech_rec_notifier {
	void (*on_result)(const char *result, char is_last);
	void (*on_speech_begin)();
	void (*on_speech_end)(int reason);	/* 0 if VAD.  others, error : see E_SR_xxx and msp_errors.h  */
};

#define END_REASON_VAD_DETECT	0	/* detected speech done  */



struct speech_rec {
	enum sr_audsrc aud_src;  /* from mic or manual  stream write */
	struct speech_rec_notifier notif;
	const char * session_id;
	int ep_stat;
	int rec_stat;
	int audio_status;
	struct recorder *recorder;
	volatile int state;
	char * session_begin_params;
};





#ifdef __cplusplus
extern "C" {
#endif

/* must init before start . devid = -1, then the default device will be used.
devid will be ignored if the aud_src is not SR_MIC */
int sr_init(struct speech_rec * sr, const char * session_begin_params, enum sr_audsrc aud_src, int devid, struct speech_rec_notifier * notifier);
int sr_start_listening(struct speech_rec *sr);
int sr_stop_listening(struct speech_rec *sr);
/* only used for the manual write way. */
int sr_write_audio_data(struct speech_rec *sr, char *data, unsigned int len);
/* must call uninit after you don't use it */
void sr_uninit(struct speech_rec * sr);

#ifdef __cplusplus
} /* extern "C" */	
#endif /* C++ */
