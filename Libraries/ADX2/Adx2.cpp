#include "pch.h"
#include "Adx2.h"
#include <locale.h>

void ADX2::Initialize(const char* acf, const char* acb)
{
	/* �G���[�R�[���o�b�N�֐��̓o�^ */
	criErr_SetCallback(user_error_callback_func);

	/* �������A���P�[�^�̓o�^ */
	criAtomEx_SetUserAllocator(user_alloc_func, user_free_func, NULL);

	/* ���C�u���������� */
	criAtomEx_Initialize_WASAPI(NULL, NULL, 0);

	/* �X�g���[�~���O�p�o�b�t�@�̍쐬 */
	m_dbas = criAtomDbas_Create(NULL, NULL, 0);

	/* �S�̐ݒ�t�@�C���̓o�^ */
	criAtomEx_RegisterAcfFile(NULL, acf, NULL, 0);

	/* DSP�o�X�ݒ�̓o�^ */
	criAtomEx_AttachDspBusSetting("DspBusSetting_0", NULL, 0);

	/* �{�C�X�v�[���̍쐬�i�X�g���[���Đ��Ή��~�j */
	CriAtomExStandardVoicePoolConfig vpconfig;
	criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vpconfig);
//	vpconfig.player_config.streaming_flag = CRI_TRUE;
	m_voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&vpconfig, NULL, 0);

	/* �T�E���h�f�[�^�̓ǂݍ��� */
	m_acb_hn = criAtomExAcb_LoadAcbFile(NULL, acb, NULL, NULL, NULL, 0);

	/* �v���[���n���h���̍쐬 */
	m_player = criAtomExPlayer_Create(NULL, NULL, 0);
}

void ADX2::Finalize()
{
	/* �v���[���n���h���̔j�� */
	criAtomExPlayer_Destroy(m_player);

	/* ACB�n���h���̉�� */
	criAtomExAcb_Release(m_acb_hn);

	/* �{�C�X�v�[���̉�� */
	criAtomExVoicePool_Free(m_voice_pool);

	/* �S�̐ݒ�t�@�C���̓o�^���� */
	criAtomEx_UnregisterAcf();

	/* ���C�u�����̏I�� */
	criAtomEx_Finalize_WASAPI();
}

void ADX2::Update()
{
	criAtomEx_ExecuteMain();
}

int ADX2::Play(int no)
{
	/* �L���[�̎w�� */
	criAtomExPlayer_SetCueId(m_player, m_acb_hn, no);
	/* �L���[�̍Đ��J�n */
	return criAtomExPlayer_Start(m_player);
}

void ADX2::Stop(int id)
{
	/* �L���[�̍Đ��J�n */
	criAtomExPlayback_Stop(id);
}

void ADX2::SetVolume(float volume)
{
	criAtomExPlayer_SetVolume(m_player, volume);
	criAtomExPlayer_UpdateAll(m_player);
}

void ADX2::user_error_callback_func(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
{
	size_t len = 0;
	wchar_t buf[128];
	const CriChar8 *errmsg;
	/* �G���[�R�[�h����G���[������ɕϊ����ăR���\�[���o�͂��� */
	errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
	setlocale(LC_ALL, "japanese");
	mbstowcs_s(&len, buf, 128, errmsg, _TRUNCATE);
	OutputDebugString(buf);
	return;
}

/* �������m�ۊ֐��̃��[�U���� */
void *ADX2::user_alloc_func(void *obj, CriUint32 size)
{
	void *ptr;
	ptr = malloc(size);
	return ptr;
}

/* ����������֐��̃��[�U���� */
void ADX2::user_free_func(void *obj, void *ptr)
{
	free(ptr);
}
