#pragma once

namespace apn::volume_meter
{
	//
	// このクラスはタイミングです。
	//
	struct timing_t
	{
		AviUtl::FileInfo fi;
		int32_t frame;
		DWORD time;

		//
		// デフォルトコンストラクタです。
		//
		timing_t ()
		{
		}

		//
		// コンストラクタです。
		// fpとfpipからタイミングを取得します。
		//
		timing_t(AviUtl::FilterPlugin* fp, AviUtl::FilterProcInfo* fpip)
		{
			fp->exfunc->get_file_info(fpip->editp, &fi);
			frame = fpip->frame;
			time = ::timeGetTime();
		}
	};
}
