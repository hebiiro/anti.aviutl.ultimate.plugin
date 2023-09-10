#pragma once

namespace Share::Nest {
	namespace Message {
		inline const UINT EraseShuttle = ::RegisterWindowMessage(_T("Nest::EraseShuttle"));
		inline const UINT RenameShuttle = ::RegisterWindowMessage(_T("Nest::RenameShuttle"));
	}
	namespace Result {
		inline const int Empty = 0;
		inline const int True = 1;
		inline const int False = 2;
	}
}
