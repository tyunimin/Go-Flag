/****************************************************************************
 *
 * CRI ADX2 LE SDK
 *
 * Copyright (c) 2013-2014 CRI Middleware Co., Ltd.
 *
 * Title   : CRI ADX2 LE sample program
 * Module  : �C���^���N�e�B�u �T�E���h
 * File    : InteractiveSound.cpp
 *
 ****************************************************************************/

/**************************************************************************
 * �C���N���[�h
 * Header files
 **************************************************************************/
#include <windows.h>
#include <cri_adx2le.h>
#include <CriSmpFramework.h>

/* �C���Q�[���v���r���[��L���ɂ���ꍇ�A��`���� */
#define USE_INGAME_PREVIEW

/**************************************************************************
 * �萔�}�N����`
 * Constant macro definitions
 **************************************************************************/
/* ACB�t�@�C���̃}�N����`�w�b�_ */
#include "../../data/Public/InteractiveSound.h"
/* ACF�t�@�C���̃}�N����`�w�b�_ */
#include "../../data/Public/ADX2_samples_acf.h"

/* �f�[�^�f�B���N�g���ւ̃p�X */
/* Header file for path definitions to data directory */
#define PATH				"../../../data/Public/"

/* �T���v���Ŏg�p����t�@�C���� */
/* File names used in this sample */
#define ACF_FILE			"ADX2_samples.acf"
#define ACB_FILE			"InteractiveSound.acb"
#define AWB_FILE			"InteractiveSound.awb"

/* �ő�{�C�X���𑝂₷���߂̊֘A�p�����[�^ */
#define MAX_VOICE			(24)
#define MAX_VIRTUAL_VOICE	(MAX_VOICE + 8)		/* �o�[�`�����{�C�X�͑��� */
#define MAX_CRIFS_LOADER	(MAX_VOICE + 8)		/* �ǂݍ��ݐ������߂� */

/* �ő�T���v�����O���[�g�i�s�b�`�ύX�܂ށj */
#define MAX_SAMPLING_RATE	(48000*2)

/* AISAC�R���g���[���l��臒l */
#define X_POS_OFFSET_FOR_SLIDER_DRAW	(30)
#define Z_POS_OFFSET_FOR_SLIDER_DRAW	(0)

#define X_POS_SCALE_VAL	(50.0f)

#define SLIDER_WIDTH	(51)

/* AISAC�R���g���[���l�̒P�ʕω��� */
#define DELTA_AISAC_VALUE (0.02f) // 1 �� X_POS_SCALE_VAL (�P�ʕω��ʂ�����A�X���C�_�[�z��̂P�v�f���ɑ����������j

/**************************************************************************
 * �^��`
 * Type definitions
 **************************************************************************/
/* AISAC�v���r���[���� */
typedef struct App3dPositioningPreviewItemTag {
	CriAtomExCueId cue_id;
} App3dPositioningPreviewItem;

/* �A�v���P�[�V�����\���� */
typedef struct AppTag{
	/* �������\�[�X�I�u�W�F�N�g */
	CriAtomExVoicePoolHn voice_pool;
	CriAtomExAcbHn	acb_hn;
	CriAtomExPlayerHn player;			/* �Đ��v���[�� */
	CriAtomExAisacControlId  aisac_control_id;
	CriAtomDbasId dbas_id;

	/* �Đ��L���[�̃v���r���[�C���f�b�N�X */
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
static void app_decrease_aisac(AppObj *app_obj);
static void app_increase_aisac(AppObj *app_obj);
static void app_reset_aisac(AppObj *app_obj);

/**************************************************************************
 * �ϐ���`
 * Variable definitions
 **************************************************************************/
/* AISAC �̃v���r���[���X�g */
static App3dPositioningPreviewItem g_preview_list[] = {
	CRI_INTERACTIVESOUND_MULTI_TRACK,
	CRI_INTERACTIVESOUND_IMPACT_SE,
};

/* AISAC�X���C�_�[�̏������ */
static const CriChar8 g_slider_default[SLIDER_WIDTH + 1] = {
	"---------------------------------------------------",
};

/* �X���C�_�[�\���p */
static CriChar8 g_slider[SLIDER_WIDTH + 1];

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

			/* ����5: AISAC�R���g���[���l������ */
			if ( idev->IsOn(idev->BUTTON_LEFT) ) {
				app_decrease_aisac(&app_obj);
			}
			/* ����6: AISAC�R���g���[���l�𑝉� */
			if ( idev->IsOn(idev->BUTTON_RIGHT) ) {
				app_increase_aisac(&app_obj);
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
	CriSmpPrintf( 2, i++, "CRI ADX2 LE Sample Program: InteractiveSound");i+=2;

	/* �Đ����̕\�� */
	CriSmpPrintf( 2, i++, "<< AISAC Preview List >>");
	for (CriSint32 n=0;n<sizeof(g_preview_list)/sizeof(App3dPositioningPreviewItem);n++) {
		if (n==app_obj->selecting_preview_index) {
			CriSmpPrintf( 1, i, "*");
		}
		CriSmpPrintf( 3, i, "%d: Cue Name=%s", n, criAtomExAcb_GetCueNameById(app_obj->acb_hn, g_preview_list[n].cue_id));
		i++;
	}
	i++;
	CriSmpPrintf( 2, i++, "Current Preview Index : %d", app_obj->current_preview_index);

	CriFloat32 aisac_val = criAtomExPlayer_GetAisacById(app_obj->player, app_obj->aisac_control_id);
	CriSmpPrintf( 2, i++, "Current AISAC Value   : %1.2f", aisac_val);

	/* �L�[���͏��̕\�� */
	i=22;
	CriSmpPrintf( 2, i++, "1          key: Start");
	CriSmpPrintf( 2, i++, "2          key: Stop");
	CriSmpPrintf( 2, i++, "UP/DOWN    Key: Select Preview Index");
	CriSmpPrintf( 2, i++, "LEFT/RIGHT Key: Change AISAC Control Value");
	CriSmpPrintf( 2, i++, "ESC        key: EXIT");

	/* ���݂�AISAC�R���g���[���l���X���C�_�[��ɕ\�� */
	i=9;
	CriSint32 mapped_aisac_cv = (CriSint32)(criAtomExPlayer_GetAisacById(app_obj->player, app_obj->aisac_control_id) * X_POS_SCALE_VAL);
	memcpy(g_slider, g_slider_default, SLIDER_WIDTH + 1);
	if ((i < SLIDER_WIDTH + 1) && (i >= 0)) {
		g_slider[mapped_aisac_cv] = '#';
	}
	CriSmpPrintf(X_POS_OFFSET_FOR_SLIDER_DRAW, i + Z_POS_OFFSET_FOR_SLIDER_DRAW, "%s", g_slider);
}

static CriBool app_atomex_initialize(AppObj *app_obj)
{
	/* �A�v���P�[�V�����̏����l�ݒ� */
	app_obj->selecting_preview_index = 0;
	app_obj->current_preview_index = 0;
	app_obj->aisac_control_id = CRI_INTERACTIVESOUND_AISACCONTROL_ANY;

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
	vpool_config.num_voices = MAX_VOICE;
	vpool_config.player_config.max_sampling_rate = MAX_SAMPLING_RATE;
	vpool_config.player_config.streaming_flag = CRI_TRUE;
	app_obj->voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&vpool_config, NULL, 0);

	/* ACB�t�@�C����ǂݍ��݁AACB�n���h�����쐬 */
	app_obj->acb_hn = criAtomExAcb_LoadAcbFile(NULL, PATH ACB_FILE, NULL, PATH AWB_FILE, NULL, 0);

	/* �v���[���̍쐬 */
	app_obj->player = criAtomExPlayer_Create(NULL, NULL, 0);
	app_reset_aisac(app_obj);

	return CRI_TRUE;
}

static CriBool app_atomex_finalize(AppObj *app_obj)
{
	/* DSP�̃f�^�b�` */
	criAtomEx_DetachDspBusSetting();

	/* Atom�n���h���̔j�� */
	criAtomExPlayer_Destroy(app_obj->player);
	
	/* �{�C�X�v�[���̔j�� */
	criAtomExVoicePool_Free(app_obj->voice_pool);

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

	return CRI_TRUE;
}

static CriBool app_atomex_start(AppObj *app_obj)
{
	/* �O�̉����~ */
	criAtomExPlayer_Stop(app_obj->player);

	/* �L���[ID�̎w�� */
	criAtomExPlayer_SetCueId(app_obj->player, app_obj->acb_hn, g_preview_list[app_obj->selecting_preview_index].cue_id);

	/* �Đ��̊J�n */
	criAtomExPlayer_Start(app_obj->player);

	app_obj->current_preview_index = app_obj->selecting_preview_index;

	return CRI_TRUE;
}

static CriBool app_atomex_stop(AppObj *app_obj)
{
	criAtomExPlayer_Stop(app_obj->player);
	app_reset_aisac(app_obj);

	return CRI_TRUE;
}

void app_reset_aisac(AppObj *app_obj)
{
	/* AISAC�R���g���[���l���[�������� */
	criAtomExPlayer_SetAisacById(app_obj->player, app_obj->aisac_control_id, 0.0f);
	return;
}

static void app_decrease_aisac(AppObj *app_obj)
{
	/* AISAC�R���g���[���l������ */
	CriFloat32 aisac_val = criAtomExPlayer_GetAisacById(app_obj->player, app_obj->aisac_control_id) - DELTA_AISAC_VALUE;
	criAtomExPlayer_SetAisacById(app_obj->player, app_obj->aisac_control_id, aisac_val);
	criAtomExPlayer_UpdateAll(app_obj->player);
}

static void app_increase_aisac(AppObj *app_obj)
{
	/* AISAC�R���g���[���l������ */
	CriFloat32 aisac_val = criAtomExPlayer_GetAisacById(app_obj->player, app_obj->aisac_control_id) + DELTA_AISAC_VALUE;
	criAtomExPlayer_SetAisacById(app_obj->player, app_obj->aisac_control_id, aisac_val);
	criAtomExPlayer_UpdateAll(app_obj->player);
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
