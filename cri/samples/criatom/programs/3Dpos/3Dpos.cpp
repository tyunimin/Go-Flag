/****************************************************************************
 *
 * CRI ADX2 LE SDK
 *
 * Copyright (c) 2013-2014 CRI Middleware Co., Ltd.
 *
 * Title   : CRI ADX2 LE sample program
 * Module  : 3D�|�W�V���j���O
 * File    : 3Dpos.cpp
 *
 ****************************************************************************/

/**************************************************************************
 * �C���N���[�h
 * Header files
 **************************************************************************/
#include <windows.h>
#include <math.h> 

#include <cri_adx2le.h>
#include <CriSmpFramework.h>

/* �C���Q�[���v���r���[��L���ɂ���ꍇ�A��`���� */
#define USE_INGAME_PREVIEW

/**************************************************************************
 * �萔�}�N����`
 * Constant macro definitions
 **************************************************************************/
/* ACF/ACB�̃}�N����`�w�b�_ */
#include "../../data/Public/3Dpos.h"
#include "../../data/Public/ADX2_samples_acf.h"

/* �f�[�^�f�B���N�g���ւ̃p�X */
/* Header file for path definitions to data directory */
#define PATH				"../../../data/Public/"

/* �T���v���Ŏg�p����t�@�C���� */
#define ACF_FILE			"ADX2_samples.acf"
#define ACB_FILE			"3Dpos.acb"

/* �ő�{�C�X���𑝂₷���߂̊֘A�p�����[�^ */
#define MAX_STANDARD_VOICE	(8)
#define MAX_HCAMX_VOICE		(8)
#define MAX_VOICE			(MAX_STANDARD_VOICE + MAX_HCAMX_VOICE)
#define MAX_VIRTUAL_VOICE	(MAX_VOICE + 8)		/* �o�[�`�����{�C�X�͑��� */
#define MAX_CRIFS_LOADER	(MAX_VOICE + 1)		/* �ǂݍ��ݐ������߂� */

/* �ő�T���v�����O���[�g�i�s�b�`�ύX�܂ށj */
#define MAX_SAMPLING_RATE	(48000*2)

/* HCA-MX�R�[�f�b�N�̃T���v�����O���[�g */
#define HCAMX_SAMPLING_RATE		(32000)

/* �����̈ړ���̍ŏ��l�ƍő�l (200 x 200 x 200 �̗����̏��ԓ����ړ�����) */
#define SOURCE_POS_MIN				(-100)
#define SOURCE_POS_MAX				(100)

#define X_POS_OFFSET_FOR_MAP_DRAW	(60)
#define Z_POS_OFFSET_FOR_MAP_DRAW	(7)

#define X_POS_SCALE_VAL				(0.2f)
#define Z_POS_SCALE_VAL				(0.08f)

#define MAP_2D_WIDTH				(41)
#define MAP_2D_HEIGHT				(17)

#define GM	(-44.0f)

/**************************************************************************
 * �^��`
 * Type definitions
 **************************************************************************/
/* 3D�����̈ړ��p�^�[�� */
typedef enum {
	APP_MORTION_CROSS_FRONT_OF_LISTENER = 0,
	APP_MOTION_PASS_LISTENER_FRONT_TO_REAR,
	APP_MOTION_GO_AROUND_LISTENER,
	APP_MOTION_APPEAR_AT_RANDOM,
} AppSourceMotionPattern;

/* 3D�|�W�V���j���O�v���r���[���� */
typedef struct App3dPositioningPreviewItemTag {
	CriAtomExCueId cue_id;
	AppSourceMotionPattern motion_pattern;
	const CriChar8 *description;
} App3dPositioningPreviewItem;

/* �A�v���P�[�V�����\���� */
typedef struct AppTag{
	/* �������\�[�X�I�u�W�F�N�g */
	CriAtomExVoicePoolHn voice_pool;
	CriAtomExVoicePoolHn hcamx_voice_pool; /* �{�C�X�v�[���n���h��(HCA-MX�R�[�f�b�N�p) 	*/
	CriAtomExAcbHn acb_hn;
	CriAtomDbasId dbas_id;

	/* 3D���� */
	CriAtomExPlayerHn player;			/* �Đ��v���[�� */
	CriAtomEx3dSourceHn source;			/* �����I�u�W�F�N�g */
	CriAtomExVector source_pos;			/* �����̈ʒu */
	CriAtomExVector source_velocity;	/* �����̑��x */

	/* ���X�i�[ */
	CriAtomEx3dListenerHn listener;
	CriAtomExVector listener_pos;		/* ���X�i�[�̈ʒu */
	CriAtomExVector listener_front;		/* ���X�i�[�̌����Ă������(�O���x�N�g��) */
	CriAtomExVector listener_top;		/* ���X�i�[�̌����Ă������(����x�N�g��) */

	/* 3D�|�W�V���j���O�̃v���r���[�C���f�b�N�X */
	CriSint32 selecting_preview_index;
	CriSint32 current_preview_index;
} AppObj;

/***************************************************************************
 * �֐���`
 * Function definitions
 ***************************************************************************/
static void user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray);
void *user_alloc_func(void *obj, CriUint32 size);
void user_free_func(void *obj, void *ptr);

/* �A�v���P�[�V�������̎��� */
static void app_display_info(AppObj *app_obj);
static CriBool app_atomex_initialize(AppObj *app_obj);
static CriBool app_atomex_finalize(AppObj *app_obj);
static CriBool app_execute_main(AppObj *app_obj);
static CriBool app_atomex_start(AppObj *app_obj);
static CriBool app_atomex_stop(AppObj *app_obj);
static void app_goto_next_preview(AppObj *app_obj);
static void app_goto_prev_preview(AppObj *app_obj);
static void app_reset_source(AppObj *app_obj);
static void app_update_source(AppObj *app_obj);

/**************************************************************************
 * �ϐ���`
 * Variable definitions
 **************************************************************************/

/* 3D Positioning�̃v���r���[���X�g */
static App3dPositioningPreviewItem g_preview_list[] = {
	{CRI__3DPOS_HELI_LOOP,		APP_MORTION_CROSS_FRONT_OF_LISTENER,    "Cross front of Listener"},
	{CRI__3DPOS_FOOTSTEP_LOOP,	APP_MOTION_PASS_LISTENER_FRONT_TO_REAR, "Pass Listener from front to rear"},
	{CRI__3DPOS_FOOTSTEP_LOOP,	APP_MOTION_GO_AROUND_LISTENER,        "Go around Listener"},
	{CRI__3DPOS_BOMB2,			APP_MOTION_APPEAR_AT_RANDOM,          "Appears at random"},
};

/* 2D�}�b�v�̏������ */
static const CriChar8 g_map_2d_default[MAP_2D_HEIGHT][MAP_2D_WIDTH + 1] = {
	"                    |z                   ", // 0
	"                    |                    ", // 1
	"                    |                    ", // 2
	"                    |                    ", // 3
	"                    |                    ", // 4
	"                    |                    ", // 5
	"                    |                    ", // 6
	"                    |                    ", // 7
	"-------------------(L)-------------------", // 8 ���_��Listener���݂�
	"                    |                   x", // 9
	"                    |                    ", //10
	"                    |                    ", //11
	"                    |                    ", //12
	"                    |                    ", //13
	"                    |                    ", //14
	"                    |                    ", //15
	"                    |                    ", //16
};

/* ���ۂɕ`�悳���2D�}�b�v�B3D������x, z�����̈ړ���2D�\������ */
static CriChar8 g_map_2d[MAP_2D_HEIGHT][MAP_2D_WIDTH + 1];

/***************************************************************************
 * �֐���`
 * Function definitions
 ***************************************************************************/

/* �T���v���̃G���g���|�C���g */
int main(void)
{
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

	/* ���C�����[�v�̊J�n */
	for (;;) {
		/* ���[�U�̃L�[���͏����X�V */
		idev->Update();
		{
			/* ����1: �����̍Đ��J�n */
			if ( idev->IsPressed(idev->BUTTON1) ) {
				app_atomex_start(&app_obj);
			}
			/* ����2: �����̍Đ���~ */
			if ( idev->IsPressed(idev->BUTTON2) ) {
				app_atomex_stop(&app_obj);
			}
			/* ����3: �Đ����[�h��O�Ɉڍs */
			if ( idev->IsPressed(idev->BUTTON_UP) ) {
				app_goto_prev_preview(&app_obj);
			}
			/* ����4: �Đ����[�h�����Ɉڍs */
			if ( idev->IsPressed(idev->BUTTON_DOWN) ) {
				app_goto_next_preview(&app_obj);
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

	return CRI_OK;
}

static void app_display_info(AppObj *app_obj)
{
	CriSint32 i = 1;
	CriSmpPrintf( 2, i++, "CRI ADX2 LE Sample Program: 3D Positioning");i+=2;

	/* �Đ����̕\�� */
	CriSmpPrintf( 2, i++, "<< 3D Positioning Preview List >>");
	for (CriSint32 n=0; n<sizeof(g_preview_list)/sizeof(App3dPositioningPreviewItem); n++) {
		if (n==app_obj->selecting_preview_index) {
			CriSmpPrintf( 1, i, "*");
		}
		CriSmpPrintf( 3, i, "%d: Cue Name=%s", n, criAtomExAcb_GetCueNameById(app_obj->acb_hn, g_preview_list[n].cue_id));
		CriSmpPrintf(27, i, "Motion Type=%s", g_preview_list[n].description);
		i++;
	}

	i++;
	CriSmpPrintf( 2, i++, "Current Preview Index : %d", app_obj->current_preview_index);

	i++;
	i++;
	CriSmpPrintf( 2, i++, "<Source Info>");
	CriSmpPrintf( 2, i++, "  Motion Type  : %s", g_preview_list[app_obj->current_preview_index].description);
	CriSmpPrintf( 2, i++, "  Position     : (%7.2f, %7.2f, %7.2f)", app_obj->source_pos.x, app_obj->source_pos.y, app_obj->source_pos.z);
	CriSmpPrintf( 2, i++, "  Velocity     : (%7.2f, %7.2f, %7.2f)", app_obj->source_velocity.x, app_obj->source_velocity.y, app_obj->source_velocity.z);

	/* �L�[���͏��̕\�� */
	i=25;
	CriSmpPrintf( 2, i++, "1       key: Start");
	CriSmpPrintf( 2, i++, "2       key: Stop");
	CriSmpPrintf( 2, i++, "UP/DOWN Key: Select Preview Index");
	CriSmpPrintf( 2, i++, "ESC     key: EXIT");

	/* 3D�����̈ʒu��2D�}�b�v��ɕ`�悷�� */
	CriSint32 mapped_source_pos_x		= (CriSint32)((app_obj->source_pos.x + SOURCE_POS_MAX) * X_POS_SCALE_VAL);
	CriSint32 mapped_source_pos_z		= (MAP_2D_HEIGHT-1) - (CriSint32)((app_obj->source_pos.z + SOURCE_POS_MAX) * Z_POS_SCALE_VAL);
	memcpy(g_map_2d, g_map_2d_default, MAP_2D_HEIGHT * (MAP_2D_WIDTH + 1));	/* �}�b�v��̃K�C�h��� */
	for (int i=0; i<MAP_2D_HEIGHT; i++) {
		if (i == mapped_source_pos_z) {
			g_map_2d[i][mapped_source_pos_x] = 'S';
		}
		CriSmpPrintf(X_POS_OFFSET_FOR_MAP_DRAW, i + Z_POS_OFFSET_FOR_MAP_DRAW, "%s", g_map_2d[i]);
	}
	CriSmpPrintf(90, i-5 , "(L:Listener)"); i++;
	CriSmpPrintf(90, i-5 , "(S:Source )");
}

static CriBool app_atomex_initialize(AppObj *app_obj)
{
	/* �A�v���P�[�V�����̏����l�ݒ� */
	app_obj->selecting_preview_index = 0;
	app_obj->current_preview_index = 0;

	/* �����̏����ʒu��ݒ�(���̎��_�ł͂܂����ۂɔ��f����Ă��Ȃ�) */
	app_obj->source_pos.x	 = 0.0f;	app_obj->source_pos.y	 = 0.0f;	app_obj->source_pos.z	 = 20.0f;

	/* ���X�i�[�̏����ʒu��ݒ�(���̎��_�ł͂܂����ۂɔ��f����Ă��Ȃ�) */
	app_obj->listener_pos.x	 = 0.0f;	app_obj->listener_pos.y	 = 0.0f;	app_obj->listener_pos.z	 = 0.0f;
	app_obj->listener_front.x = 0.0f;	app_obj->listener_front.y = 0.0f;	app_obj->listener_front.z = 1.0f;
	app_obj->listener_top.x	 = 0.0f;	app_obj->listener_top.y	 = 1.0f;	app_obj->listener_top.z	 = 0.0f;

	/* �G���[�R�[���o�b�N�֐��̓o�^ */
	criErr_SetCallback(user_error_callback_func);

	/* �������A���P�[�^�̓o�^ */
	criAtomEx_SetUserAllocator(user_alloc_func, user_free_func, NULL);

	/* ���C�u�����̏������ݒ�i�ő�{�C�X���ύX�j */
	CriAtomExConfig_WASAPI lib_config;
	CriFsConfig fs_config;
	criAtomEx_SetDefaultConfig_WASAPI(&lib_config);
	criFs_SetDefaultConfig(&fs_config);
	lib_config.atom_ex.max_virtual_voices = MAX_VIRTUAL_VOICE;
	lib_config.hca_mx.output_sampling_rate = HCAMX_SAMPLING_RATE;
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
	CriAtomExStandardVoicePoolConfig vpool_config;
	criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vpool_config);
	vpool_config.num_voices							= MAX_STANDARD_VOICE;
	vpool_config.player_config.max_sampling_rate	= MAX_SAMPLING_RATE;
	vpool_config.player_config.streaming_flag		= CRI_TRUE;
	app_obj->voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&vpool_config, NULL, 0);

	/* HCA-MX�Đ��p�̃{�C�X�v�[���쐬 */
	CriAtomExHcaMxVoicePoolConfig hcamx_config;
	criAtomExVoicePool_SetDefaultConfigForHcaMxVoicePool(&hcamx_config);
	hcamx_config.num_voices							= MAX_HCAMX_VOICE;
	hcamx_config.player_config.max_sampling_rate	= HCAMX_SAMPLING_RATE;
	hcamx_config.player_config.streaming_flag		= CRI_TRUE;
	app_obj->hcamx_voice_pool = criAtomExVoicePool_AllocateHcaMxVoicePool(&hcamx_config, NULL, 0);

	/* ACB�t�@�C����ǂݍ��݁AACB�n���h�����쐬 */
	app_obj->acb_hn = criAtomExAcb_LoadAcbFile(NULL, PATH ACB_FILE, NULL, NULL, NULL, 0);

	/* �v���[���̍쐬 */
	app_obj->player = criAtomExPlayer_Create(NULL, NULL, 0);

	/* 3D���X�i�[�n���h����3D�����n���h�����쐬 */
	app_obj->listener = criAtomEx3dListener_Create(NULL, NULL, 0);
	app_obj->source = criAtomEx3dSource_Create(NULL, NULL, 0);

	/* �v���[���Ƀ��X�i�[�Ɖ����̃n���h����o�^ */
	criAtomExPlayer_Set3dListenerHn(app_obj->player, app_obj->listener);
	criAtomExPlayer_Set3dSourceHn(app_obj->player, app_obj->source);

	/* ���X�i�[�̈ʒu�̐ݒ� */
	criAtomEx3dListener_SetPosition(app_obj->listener, &(app_obj->listener_pos)); 

	/* ���X�i�[�̌����̐ݒ� */
	criAtomEx3dListener_SetOrientation(app_obj->listener, &(app_obj->listener_front), &(app_obj->listener_top));

	/* ���X�i�[�̃p�����[�^�����ۂɔ��f */
	criAtomEx3dListener_Update(app_obj->listener);

	/* �����̈ʒu�̐ݒ聕���f */	
	criAtomEx3dSource_SetPosition(app_obj->source, &(app_obj->source_pos));

	/* �����̃p�����[�^�����ۂɔ��f */
	criAtomEx3dSource_Update(app_obj->source);

	return CRI_TRUE;
}

static CriBool app_atomex_finalize(AppObj *app_obj)
{
	/* DSP�̃f�^�b�` */
	criAtomEx_DetachDspBusSetting();

	/* �v���[���n���h���̔j�� */
	criAtomExPlayer_Destroy(app_obj->player);
	
	/* �{�C�X�v�[���̔j�� */
	criAtomExVoicePool_Free(app_obj->voice_pool);
	criAtomExVoicePool_Free(app_obj->hcamx_voice_pool);

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
	CriAtomExPlayerStatus player_status;

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

	/* Ex�v���[���̃X�e�[�^�X�m�F */
	player_status = criAtomExPlayer_GetStatus(app_obj->player);

	/* �Đ��G���[�ɂȂ����ꍇ�̓A�v���P�[�V�������I�� */
	if (player_status == CRIATOMEXPLAYER_STATUS_ERROR) {
		return CRI_FALSE;		
	}
		
	/* �����̈ʒu�̍X�V */
	if (player_status == CRIATOMEXPLAYER_STATUS_PLAYING) {
		app_update_source(app_obj);
	}
	
	return CRI_TRUE;
}

static CriBool app_atomex_start(AppObj *app_obj)
{
	/* �O�̉����~ */
	//criAtomExPlayer_Stop(app_obj->player);
	criAtomExPlayer_StopWithoutReleaseTime(app_obj->player);

	/* �L���[ID�̎w�� */
	criAtomExPlayer_SetCueId(app_obj->player, app_obj->acb_hn, g_preview_list[app_obj->selecting_preview_index].cue_id);

	/* �Đ��̊J�n */
	criAtomExPlayer_Start(app_obj->player);
	
	/* �ړ��p�^�[����ύX���ĉ����������ʒu�ɒu�� */
	app_obj->current_preview_index = app_obj->selecting_preview_index;
	app_reset_source(app_obj);

	return CRI_TRUE;
}

static CriBool app_atomex_stop(AppObj *app_obj)
{
	criAtomExPlayer_Stop(app_obj->player);
	app_reset_source(app_obj);

	return CRI_TRUE;
}

void app_reset_source(AppObj *app_obj)
{
	/* �����̏����ʒu��ݒ� */
	switch (g_preview_list[app_obj->current_preview_index].motion_pattern) {
		case	APP_MORTION_CROSS_FRONT_OF_LISTENER:
			app_obj->source_pos.x	 =-100.0f;		app_obj->source_pos.y	 = 0.0f;		app_obj->source_pos.z	 = 40.0f;
			app_obj->source_velocity.x=0.25f;		app_obj->source_velocity.y= 0.0f;		app_obj->source_velocity.z= 0.0f;
			break;
		case	APP_MOTION_PASS_LISTENER_FRONT_TO_REAR:
			app_obj->source_pos.x	 =   0.0f;		app_obj->source_pos.y	 = 0.0f;		app_obj->source_pos.z	 =100.0f;
			app_obj->source_velocity.x=   0.0f;		app_obj->source_velocity.y= 0.0f;		app_obj->source_velocity.z= -0.25f;
			break;
		case	APP_MOTION_GO_AROUND_LISTENER:
			app_obj->source_pos.x	 =50.0f;		app_obj->source_pos.y	 = 0.0f;		app_obj->source_pos.z	 = 0.0f;
			app_obj->source_velocity.x=0.0f;			app_obj->source_velocity.y= 0.0f;		app_obj->source_velocity.z= 1.0f;
			break;
		case	APP_MOTION_APPEAR_AT_RANDOM:				
			app_obj->source_pos.x	 = (CriFloat32)(rand() % (SOURCE_POS_MAX*2) - SOURCE_POS_MAX);
			app_obj->source_pos.y	 = 0.0f;
			app_obj->source_pos.z	 = (CriFloat32)(rand() % (SOURCE_POS_MAX*2) - SOURCE_POS_MAX);
			app_obj->source_velocity.x=   0.0f;		app_obj->source_velocity.y= 0.0f;		app_obj->source_velocity.z= 0.0f;
			break;
		default:
			app_obj->source_pos.x	 =   0.0f;		app_obj->source_pos.y	 = 0.0f;		app_obj->source_pos.z	 =20.0f;
			app_obj->source_velocity.x= 0.0f;		app_obj->source_velocity.y= 0.0f;		app_obj->source_velocity.z= 0.0f;
			break;
	}

	/* ��Őݒ肵���p�����[�^�����ۂɓo�^�����f������ */
	criAtomEx3dSource_SetVelocity (app_obj->source, &(app_obj->source_velocity)); 
	criAtomEx3dSource_SetPosition(app_obj->source, &(app_obj->source_pos));
	criAtomEx3dSource_Update(app_obj->source);
	return;
}

void app_update_source(AppObj *app_obj)
{
	switch (g_preview_list[app_obj->current_preview_index].motion_pattern) {
		case	APP_MORTION_CROSS_FRONT_OF_LISTENER:
			/* ���X�i�[�̑O�����؂� */
			if (app_obj->source_pos.x < SOURCE_POS_MAX) {
				app_obj->source_pos.x = app_obj->source_pos.x + app_obj->source_velocity.x;
			} else {
				/* �ړ���̒[�܂ŒH�蒅�����珉���ʒu�ɖ߂� */
				app_reset_source(app_obj);
			}
			break;
		case	APP_MOTION_PASS_LISTENER_FRONT_TO_REAR:
			/* ���X�i�[�̐��ʂ���w��ւƒʂ蔲���� */
			if (app_obj->source_pos.z >= SOURCE_POS_MIN) {
				app_obj->source_pos.z = app_obj->source_pos.z + app_obj->source_velocity.z;
			} else {
				app_reset_source(app_obj);
			}
			break;
		case	APP_MOTION_GO_AROUND_LISTENER:
			/* ���X�i�[(���_)�̎��͂���� */
			{
				CriFloat32 rr = static_cast<CriFloat32>(pow(app_obj->source_pos.x, 2.0f) + pow(app_obj->source_pos.z, 2.0f));
				CriFloat32 acceleration = GM / rr;
				CriFloat32 r = static_cast<CriFloat32>(sqrt(rr));

				app_obj->source_velocity.x = app_obj->source_velocity.x + (acceleration * app_obj->source_pos.x / r);
				app_obj->source_velocity.z = app_obj->source_velocity.z + (acceleration * app_obj->source_pos.z / r);

				app_obj->source_pos.x = app_obj->source_pos.x + app_obj->source_velocity.x;
				app_obj->source_pos.z = app_obj->source_pos.z + app_obj->source_velocity.z;
			}
			break;
		case APP_MOTION_APPEAR_AT_RANDOM:
			/* ���̃��[�h�ł͈ړ����Ȃ��B�����ʒu�������_���Ɍ��܂邾�� */
			break;
		default:
			break;
	}

	/* ��Őݒ肵���p�����[�^�����ۂɓo�^�����f������ */
	criAtomEx3dSource_SetVelocity (app_obj->source, &(app_obj->source_velocity)); 
	criAtomEx3dSource_SetPosition(app_obj->source, &(app_obj->source_pos));
	criAtomEx3dSource_Update(app_obj->source);

	return;
}

static void app_goto_next_preview(AppObj *app_obj)
{
	CriSint32 max_items = sizeof(g_preview_list)/sizeof(App3dPositioningPreviewItem);

	app_obj->selecting_preview_index++;
	if (app_obj->selecting_preview_index >= max_items) {
		app_obj->selecting_preview_index = 0;
	}
}

static void app_goto_prev_preview(AppObj *app_obj)
{
	CriSint32 max_items = sizeof(g_preview_list)/sizeof(App3dPositioningPreviewItem);

	app_obj->selecting_preview_index--;
	if (app_obj->selecting_preview_index < 0) {
		app_obj->selecting_preview_index = max_items -1;
	}
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
/* --- end of file --- */
