#pragma once

#define MY_TRACE_OBJECT_HOLDER(object) \
do { \
	MY_TRACE(_T(#object) _T(" = %d\n"), object.getObjectIndex()); \
} while (0)

namespace fgo
{
	struct ObjectHolder
	{
		int objectIndex;
		ExEdit::Object* object;

		ObjectHolder()
			: objectIndex(-1)
			, object(0)
		{
		}

		ObjectHolder(int objectIndex)
			: objectIndex(-1)
			, object(0)
		{
			this->objectIndex = objectIndex;
			if (this->objectIndex >= 0)
			{
				this->object = magi.auin.GetObject(this->objectIndex);
				if (!this->object)
					this->objectIndex = -1;
			}
		}

		int getObjectIndex() const
		{
			return objectIndex;
		}

		ExEdit::Object* getObject() const
		{
			return object;
		}

		BOOL isValid() const
		{
			if (objectIndex < 0) return FALSE;
			if (!object) return FALSE;
			return TRUE;
		}

		bool operator==(const ObjectHolder& x) const
		{
			return objectIndex == x.objectIndex && object == x.object;
		}

		bool operator!=(const ObjectHolder& x) const
		{
			return !operator==(x);
		}
	};
}
