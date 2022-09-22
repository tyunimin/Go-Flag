/****************************************************************************
 *
 * CRI ADX2 LE SDK
 *
 * Copyright (c) 2013-2014 CRI Middleware Co., Ltd.
 *
 * Title   : CRI ADX2 LE sample program
 * Module  : ��{�Đ�����T���v��(HCA-MX�Đ��Ή�)
 * File    : Basic.cpp
 *
 ****************************************************************************/

/**************************************************************************
 * �C���N���[�h
 * Header files
 **************************************************************************/
#include <cri_adx2le.h>
#include <CriSmpFramework.h>

/* �C���Q�[���v���r���[��L���ɂ���ꍇ�A��`���� */
#define USE_INGAME_PREVIEW

/**************************************************************************
 * �萔�}�N����`
 * Constant macro definitions
 **************************************************************************/
/* ACF/ACB�̃}�N����`�w�b�_ */
#include "../../data/Public/Basic.h"
#include "../../data/Public/ADX2_samples_acf.h"

/* �f�[�^�f�B���N�g���ւ̃p�X */
#define PATH	"../../../data/Public/"

/* �T���v���Ŏg�p����t�@�C���� */
#define ACF_FILE			"ADX2_samples.acf"
#define ACB_FILE			"Basic.acb"
#define AWB_FILE			"Basic.awb"

/* �ő�{�C�X���𑝂₷���߂̊֘A�p�����[�^ */
#define MAX_VOICE			(24)
#define MAX_VIRTUAL_VOICE	(64)		/* �������葽��(�ʏ�{�C�X�{HCA-MX�{�C�X�{��) */
#define MAX_CRIFS_LOADER	(64)		/* �������葽��(�ʏ�{�C�X�{HCA-MX�{�C�X�{��) */

/* �ő�T���v�����O���[�g�i�s�b�`�ύX�܂ށj */
#define MAX_SAMPLING_RATE	(48000*2)

/* HCA-MX�R�[�f�b�N�̃T���v�����O���[�g */
#define SAMPLINGRATE_HCAMX		(32000)


#define X_POS_OFFSET_FOR_MAP_DRAW	(20)
#define PITCH_CHANGE_VALUE			(-200.0f)

/**************************************************************************
 * �^��`
 * Type definitions
 **************************************************************************/

/* �L���[���X�g�A�C�e�� */
typedef struct AppCueListItemTag {
	CriAtomExCueId id;
} AppCueListItem;

/* �A�v���P�[�V�����\���� */
typedef struct AppTag{
	CriAtomExPlayerHn		player;		/* �Đ��v���[�� */
	CriAtomExVoicePoolHn	standard_voice_pool;	/* �{�C�X�v�[��(ADX/HCA�R�[�f�b�N�p) */
	CriAtomExVoicePoolHn	hcamx_voice_pool;		/* �{�C�X�v�[��(HCA-MX�p) */
	CriAtomExAcbHn			acb_hn;		/* ACB�n���h��(�����f�[�^) */
	CriAtomDbasId			dbas_id;	/* D-BAS�̍쐬*/

	CriAtomExPlaybackId		playback_id;	/* Voice�L���[�̃v���C�o�b�NID(�Đ��J�n���ɕێ�����) */

	CriSint32 ui_cue_idnex;				/* ���[�U�I�𒆂̃L���[   */

} AppObj;

/***************************************************************************
 * �֐��錾
 * Function declarations
 ***************************************************************************/
static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray);
static void *user_alloc_func(void *obj, CriUint32 size);
static void user_free_func(void *obj, void *ptr);

static void app_display_info(AppObj *app_obj);
static CriBool app_atomex_initialize(AppObj *app_obj);
static CriBool app_atomex_finalize(AppObj *app_obj);
static CriBool app_execute_main(AppObj *app_obj);

static CriBool app_atomex_start(AppObj *app_obj);
static CriBool app_atomex_stop_player(AppObj *app_obj);
static CriBool app_atomex_stop_cue(AppObj *app_obj);

/**************************************************************************
 * �ϐ���`
 * Variable definitions
 **************************************************************************/

/* �L���[�ꗗ */
static AppCueListItem g_cue_list[] = {
	CRI_BASIC_MUSIC1,
	CRI_BASIC_MUSIC2,
	CRI_BASIC_VOICE_RANDOM,
	CRI_BASIC_VOICE_EFFECT,
	CRI_BASIC_KALIMBA,
	CRI_BASIC_GUNSHOT,
	CRI_BASIC_BOMB_LIMIT,
	CRI_BASIC_PITCH_UP_M2,
	CRI_BASIC_PITCH_DEF_M2
};
static CriUint32 g_num_cue_items = sizeof(g_cue_list)/sizeof(AppCueListItem);

static CriChar8* g_playback_status_description[] = {
	"--",
	"Prepare",
	"Playing",
	"Removed",
};

/***************************************************************************
 * �֐���`
 * Function definitions
 ***************************************************************************/

int main(void) {
	/* �A�v���P�[�V�����I�u�W�F�N�g�̒�` */
	AppObj app_obj = {0};
	/* �T���v���t���[�����[�N�I�u�W�F�N�g (�E�B���h�E�Ǘ��E�f�o�b�O�����\���p) */
	CriSmpFramework *smpfw = NULL;
	/* �L�[���͔���p�I�u�W�F�N�g */
	CriSmpInputDevice* idev = NULL;

	/* �T���v���t���[�����[�N�̍쐬 */
	smpfw = CriSmpFramework::Create();
	idev = smpfw->GetInputDevice();

	/* �A�v���P�[�V�����̏����� */
	app_atomex_initialize(&app_obj);

	for (;;) {
		/* ���[�U�̃L�[���͏����X�V */
		idev->Update();
		{
			/* ����1: �L���[�Đ� */
			if ( idev->IsPressed(idev->BUTTON1) ) {
				app_atomex_start(&app_obj);
			}
			/* ����2: �v���[����~ (�S�̒�~) */
			if ( idev->IsPressed(idev->BUTTON2) ) {
				app_atomex_stop_player(&app_obj);
			}
			/* ����3: ����̍Đ����ɑ΂����~ (�{�T���v���ł�Voice�L���[�̂ݒ�~) */
			if ( idev->IsPressed(idev->BUTTON3) ) {
				app_atomex_stop_cue(&app_obj);
			}

			/* ����4: �L���[�I���i��) */
			if ( idev->IsPressed(idev->BUTTON_UP) ) {
				app_obj.ui_cue_idnex--;
				if (app_obj.ui_cue_idnex<0) {
					app_obj.ui_cue_idnex = g_num_cue_items - 1;
				}
			}
			/* ����5: �L���[�I���i��) */
			if ( idev->IsPressed(idev->BUTTON_DOWN) ) {
				app_obj.ui_cue_idnex++;
				if (app_obj.ui_cue_idnex == g_num_cue_items) {
					app_obj.ui_cue_idnex = 0;
				}
			}
		}

		/* �A�v���P�[�V�����I������ (ESC�L�[���E�B���h�E�����ꂽ�ꍇ) */
		if ( idev->IsPressed(idev->BUTTON_CANCEL) || smpfw->IsTerninated() == TRUE) {
			break;
		}

		/* �A�v���P�[�V�����̍X�V */
		if (app_execute_main(&app_obj) != CRI_TRUE) {
			break;
		}

		/* �A�v���P�[�V�����̏���\�� */
		smpfw->BeginDraw();
		app_display_info(&app_obj);
		smpfw->EndDraw();

		/* ���zV�����҂�(�^�C�}�[�ɂ��X���[�v����) */
		smpfw->SyncFrame();
	}

	/* �A�v���P�[�V�����̏I�� */
	app_atomex_finalize(&app_obj);

	/* �T���v���t���[�����[�N�̔j�� */
	smpfw->Destroy(); smpfw = NULL;

	return 0;
}

/* �G���[�R�[���o�b�N�֐� */
static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
{
	const CriChar8 *errmsg;

	/* �G���[������̕\�� */
	errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
	CriSmpDebugPrintf("%s\n", errmsg);

	return;
}

void *user_alloc_func(void *obj, CriUint32 size)
{
	void *ptr;
	ptr = malloc(size);
	return ptr;
}

void user_free_func(void *obj, void *ptr)
{
	free(ptr);
}

static void app_display_info(AppObj *app_obj)
{
	CriSint32 i = 1;
	CriSmpPrintf( 2, i++, "CRI ADX2 LE Sample Program: Basic");i++;

	/* �{�C�X�v�[�����̃{�C�X�g�p�󋵂�\�� */
	CriSint32 current_vnum, vlimit;
	criAtomExVoicePool_GetNumUsedVoices(app_obj->standard_voice_pool, &current_vnum, &vlimit);
	CriSmpPrintf( 2, i++, "Standard Voice (Playing / Max): %d/%d", current_vnum, vlimit);
	criAtomExVoicePool_GetNumUsedVoices(app_obj->hcamx_voice_pool, &current_vnum, &vlimit);
	CriSmpPrintf( 2, i++, "HCA-MX Voice   (Playing / Max): %d/%d", current_vnum, vlimit);i+=2;

	/* �L���[�̃��X�g����\�� */
	CriSmpPrintf( 2, i++, "-- Cue List --");
	for (CriUint32 n=0;n<g_num_cue_items;n++) {

		if (n==app_obj->ui_cue_idnex) {
			CriSmpPrintf(2, i+n, "*");
		}

		CriSmpPrintf( 4, i+n, "%s", criAtomExAcb_GetCueNameById(app_obj->acb_hn, g_cue_list[n].id));

		/* Voice�L���[�̖��O�ׂ̗ɂ����A���̃L���[�̍Đ���ԂƎ�����\������ */
		if (g_cue_list[n].id == CRI_BASIC_VOICE_RANDOM) {
			CriAtomExPlaybackStatus playback_state = criAtomExPlayback_GetStatus(app_obj->playback_id);
			CriSint64 playback_time = criAtomExPlayback_GetTime(app_obj->playback_id);
			CriSmpPrintf( 4 + X_POS_OFFSET_FOR_MAP_DRAW, i+n, "[Status: %s,  Time:%lld msec.]",
				g_playback_status_description[playback_state], playback_time);
		}
	}

	/* �L�[���͏��̕\�� */
	i=24;
	CriSmpPrintf( 2, i++, "1       key: Start");
	CriSmpPrintf( 2, i++, "2       key: Stop All");
	CriSmpPrintf( 2, i++, "3       key: Stop Only Voice");
	CriSmpPrintf( 2, i++, "UP/DOWN Key: Select Preview Index");
	CriSmpPrintf( 2, i++, "ESC     key: EXIT");
}

CriBool app_atomex_initialize(AppObj *app_obj)
{
	/* �J�[�\���̃��Z�b�g */
	app_obj->ui_cue_idnex = 0;
	/* ���擾�ȃv���C�o�b�NID(Voice�L���[�Đ����̂ݎ擾) */
	app_obj->playback_id = 0;

	/* �G���[�R�[���o�b�N�֐��̓o�^ */
	criErr_SetCallback(user_error_callback_func);

	/* �������A���P�[�^�̓o�^ */
	criAtomEx_SetUserAllocator(user_alloc_func, user_free_func, NULL);

	/* ���C�u�����̏������i�ő�{�C�X���ύX�j */
	CriAtomExConfig_WASAPI lib_config;
	CriFsConfig fs_config;
	criAtomEx_SetDefaultConfig_WASAPI(&lib_config);
	criFs_SetDefaultConfig(&fs_config);
	lib_config.atom_ex.max_virtual_voices = MAX_VIRTUAL_VOICE;
	lib_config.hca_mx.output_sampling_rate = SAMPLINGRATE_HCAMX;
	fs_config.num_loaders = MAX_CRIFS_LOADER;
	lib_config.atom_ex.fs_config = &fs_config;
	criAtomEx_Initialize_WASAPI(&lib_config, NULL, 0);

	/* D-Bas�̍쐬�i�ő�X�g���[�����͂����Ō��܂�܂��j */
	app_obj->dbas_id = criAtomDbas_Create(NULL, NULL, 0);

	#if defined(USE_INGAME_PREVIEW)
		CriAtomExAsrBusAnalyzerConfig analyze_config;
		CriSint32 bus_no;
		/* �C���Q�[���v���r���[�p�̃��j�^���C�u������������ */
		criAtomExMonitor_Initialize(NULL, NULL, 0);
		/* ���x������@�\��ǉ� */
		criAtomExAsr_SetDefaultConfigForBusAnalyzer(&analyze_config);
		for ( bus_no=0;bus_no<8;bus_no++) {
			criAtomExAsr_AttachBusAnalyzer(bus_no, &analyze_config);
		}
	#endif

	/* ACF�t�@�C���̓ǂݍ��݂Ɠo�^ */
	criAtomEx_RegisterAcfFile(NULL, PATH ACF_FILE, NULL, 0);
	
	/* DSP�ݒ�̃A�^�b�` */
	criAtomEx_AttachDspBusSetting(CRI_ADX2_SAMPLES_DSPSETTING_DSPBUSSETTING_0, NULL, 0);

	/* �{�C�X�v�[���̍쐬�i�ő�{�C�X���ύX�^�ő�s�b�`�ύX�^�X�g���[���Đ��Ή��j */
	CriAtomExStandardVoicePoolConfig standard_vpool_config;
	criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&standard_vpool_config);
	standard_vpool_config.num_voices = MAX_VOICE;
	standard_vpool_config.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
	standard_vpool_config.player_config.streaming_flag = CRI_TRUE;
	app_obj->standard_voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&standard_vpool_config, NULL, 0);

	/* HCA-MX�Đ��p�F�{�C�X�v�[���̍쐬 */
	CriAtomExHcaMxVoicePoolConfig hcamx_vpool_config;
	criAtomExVoicePool_SetDefaultConfigForHcaMxVoicePool(&hcamx_vpool_config);
	hcamx_vpool_config.num_voices = MAX_VOICE;
	hcamx_vpool_config.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
	hcamx_vpool_config.player_config.streaming_flag = CRI_TRUE;
	app_obj->hcamx_voice_pool = criAtomExVoicePool_AllocateHcaMxVoicePool(&hcamx_vpool_config, NULL, 0);

	/* ACB�t�@�C����ǂݍ��݁AACB�n���h�����쐬 */
	app_obj->acb_hn = criAtomExAcb_LoadAcbFile(NULL, PATH ACB_FILE, NULL, PATH AWB_FILE, NULL, 0);

	/* �v���[���̍쐬 */
	app_obj->player = criAtomExPlayer_Create(NULL, NULL, 0);

	return CRI_TRUE;
}

CriBool app_atomex_finalize(AppObj *app_obj)
{
	/* DSP�̃f�^�b�` */
	criAtomEx_DetachDspBusSetting();

	/* �v���[���n���h���̔j�� */
	criAtomExPlayer_Destroy(app_obj->player);
	
	/* �{�C�X�v�[���̔j�� */
	criAtomExVoicePool_Free(app_obj->hcamx_voice_pool);
	criAtomExVoicePool_Free(app_obj->standard_voice_pool);

	/* ACB�n���h���̔j�� */
	criAtomExAcb_Release(app_obj->acb_hn);

	/* ACF�̓o�^���� */
	criAtomEx_UnregisterAcf();

	#if defined(USE_INGAME_PREVIEW)
		/* �C���Q�[���v���r���[�֘A�@�\�̏I������ */
		CriSint32 bus_no;
		for ( bus_no=0;bus_no<8;bus_no++) {
			criAtomExAsr_DetachBusAnalyzer(bus_no);
		}
		criAtomExMonitor_Finalize();
	#endif

	/* D-BAS�̔j�� */
	criAtomDbas_Destroy(app_obj->dbas_id);

	/* ���C�u�����̏I�� */
	criAtomEx_Finalize_WASAPI();

	return CRI_TRUE;
}

CriBool app_execute_main(AppObj *app_obj)
{
	/* �T�[�o�����̎��s */
	criAtomEx_ExecuteMain();

#if defined(USE_INGAME_PREVIEW)
	/* �o�X��͏��̎擾 */
	CriAtomExAsrBusAnalyzerInfo analyze_info;
	CriSint32 bus_no;
	for ( bus_no=0;bus_no<8;bus_no++) {
		criAtomExAsr_GetBusAnalyzerInfo(bus_no, &analyze_info);
	}
#endif

	return CRI_TRUE;
}

static CriBool app_atomex_start(AppObj *app_obj)
{
	CriAtomExCueId start_cue_id = g_cue_list[app_obj->ui_cue_idnex].id;

	/* �L���[ID�̎w�� */
	criAtomExPlayer_SetCueId(app_obj->player, app_obj->acb_hn, start_cue_id);

	/* MEMO: ����̉������s�b�`��ς��čĐ��������ꍇ�B      */
	/* (1) �v���[���Ƀs�b�`��ݒ�B                          */
	/* (2) �Đ��J�n�B                                        */
	/* (3) �v���[���̃s�b�`��߂��B                          */
	/* {                                                     */
	/*   criAtomExPlayer_SetPitch(player, pitch);          */
	/*   criAtomExPlayer_SetCueId(player, acb, cue_id);  */
	/*   criAtomExPlayer_Start(player, pitch);             */
	/*   criAtomExPlayer_SetPitch(player, 0.0f);           */
	/* {                                                     */
	/* �⑫: HCA-MX�R�[�f�b�N�̏ꍇ�̓s�b�`�ύX�͖����B      */

	/* �Đ��̊J�n */
	CriAtomExPlaybackId playback_id = criAtomExPlayer_Start(app_obj->player);

	/* Voice�L���[�̂݁A�Đ������擾�̂��߃v���C�o�b�NID��ۑ����� */
	if (start_cue_id == CRI_BASIC_VOICE_RANDOM) {
		app_obj->playback_id = playback_id;
	}

	return CRI_TRUE;
}

static CriBool app_atomex_stop_player(AppObj *app_obj)
{
	/* �v���[���̒�~ */
	criAtomExPlayer_Stop(app_obj->player);

	return CRI_TRUE;
}

static CriBool app_atomex_stop_cue(AppObj *app_obj)
{
	/* ����̍Đ����̂ݒ�~ */
	criAtomExPlayback_Stop(app_obj->playback_id);

	return CRI_TRUE;
}
