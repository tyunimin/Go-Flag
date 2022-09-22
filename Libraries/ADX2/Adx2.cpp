#include "pch.h"
#include "Adx2.h"
#include <locale.h>

void ADX2::Initialize(const char* acf, const char* acb)
{
	/* エラーコールバック関数の登録 */
	criErr_SetCallback(user_error_callback_func);

	/* メモリアロケータの登録 */
	criAtomEx_SetUserAllocator(user_alloc_func, user_free_func, NULL);

	/* ライブラリ初期化 */
	criAtomEx_Initialize_WASAPI(NULL, NULL, 0);

	/* ストリーミング用バッファの作成 */
	m_dbas = criAtomDbas_Create(NULL, NULL, 0);

	/* 全体設定ファイルの登録 */
	criAtomEx_RegisterAcfFile(NULL, acf, NULL, 0);

	/* DSPバス設定の登録 */
	criAtomEx_AttachDspBusSetting("DspBusSetting_0", NULL, 0);

	/* ボイスプールの作成（ストリーム再生対応×） */
	CriAtomExStandardVoicePoolConfig vpconfig;
	criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vpconfig);
//	vpconfig.player_config.streaming_flag = CRI_TRUE;
	m_voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&vpconfig, NULL, 0);

	/* サウンドデータの読み込み */
	m_acb_hn = criAtomExAcb_LoadAcbFile(NULL, acb, NULL, NULL, NULL, 0);

	/* プレーヤハンドルの作成 */
	m_player = criAtomExPlayer_Create(NULL, NULL, 0);
}

void ADX2::Finalize()
{
	/* プレーヤハンドルの破棄 */
	criAtomExPlayer_Destroy(m_player);

	/* ACBハンドルの解放 */
	criAtomExAcb_Release(m_acb_hn);

	/* ボイスプールの解放 */
	criAtomExVoicePool_Free(m_voice_pool);

	/* 全体設定ファイルの登録解除 */
	criAtomEx_UnregisterAcf();

	/* ライブラリの終了 */
	criAtomEx_Finalize_WASAPI();
}

void ADX2::Update()
{
	criAtomEx_ExecuteMain();
}

int ADX2::Play(int no)
{
	/* キューの指定 */
	criAtomExPlayer_SetCueId(m_player, m_acb_hn, no);
	/* キューの再生開始 */
	return criAtomExPlayer_Start(m_player);
}

void ADX2::Stop(int id)
{
	/* キューの再生開始 */
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
	/* エラーコードからエラー文字列に変換してコンソール出力する */
	errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
	setlocale(LC_ALL, "japanese");
	mbstowcs_s(&len, buf, 128, errmsg, _TRUNCATE);
	OutputDebugString(buf);
	return;
}

/* メモリ確保関数のユーザ実装 */
void *ADX2::user_alloc_func(void *obj, CriUint32 size)
{
	void *ptr;
	ptr = malloc(size);
	return ptr;
}

/* メモリ解放関数のユーザ実装 */
void ADX2::user_free_func(void *obj, void *ptr)
{
	free(ptr);
}
