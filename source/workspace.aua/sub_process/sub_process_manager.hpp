#pragma once

namespace apn::workspace
{
	//
	// このクラスはサブプロセスを管理します。
	//
	inline struct SubProcessManager
	{
		inline static std::shared_ptr<SubProcess> create_sub_process(const std::wstring& type)
		{
			if (type == L"console") return console;
			if (type == L"psdtoolkit") return psdtoolkit;
			return std::make_shared<SubProcess>();
		}

		//
		// 初期化処理を実行します。
		//
		BOOL init()
		{
			MY_TRACE_FUNC("");

			for (const auto& node : hive.sub_process.collection)
			{
				if (!node->active) continue;

				auto sub_process = create_sub_process(node->type);

				sub_process->create(node);
			}

			return TRUE;
		}

		//
		// 後始末処理を実行します。
		//
		BOOL exit()
		{
			MY_TRACE_FUNC("");

			return TRUE;
		}
	} sub_process_manager;
}
