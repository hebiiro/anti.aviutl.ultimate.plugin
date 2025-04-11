#pragma once

#define MY_TRACE_OBJECT_HOLDER(object) \
do { \
	MY_TRACE(#object _T(" = {/}\n"), object.get_object_index()); \
} while (false)

namespace apn
{
	struct ObjectHolder
	{
		int32_t object_index = -1;
		ExEdit::Object* object = nullptr;

		ObjectHolder()
		{
		}

		ObjectHolder(int32_t object_index)
		{
			this->object_index = object_index;
			if (this->object_index >= 0)
			{
				this->object = magi.exin.get_object(this->object_index);
				if (!this->object)
					this->object_index = -1;
			}
		}

		int32_t get_object_index() const
		{
			return object_index;
		}

		ExEdit::Object* get_object() const
		{
			return object;
		}

		BOOL is_valid() const
		{
			if (object_index < 0) return FALSE;
			if (!object) return FALSE;
			return TRUE;
		}

		bool operator==(const ObjectHolder& x) const
		{
			return object_index == x.object_index && object == x.object;
		}

		bool operator!=(const ObjectHolder& x) const
		{
			return !operator==(x);
		}
	};
}
