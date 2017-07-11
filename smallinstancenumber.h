#pragma once

namespace ams
{

#if 0
}// fool visual assist
#endif 
namespace SmallInstanceNumber
{
#if 0
}// fool visual assist
#endif 

const int MaxInstances = 65536;

class FreeStackError
{
public:
    enum Errors
    {
        INSTANCES_EXCEEDED,
        INSTANCES_LESS_THAN_ZERO
    };
    FreeStackError(Errors error) : error_(error) {}
private:
    Errors error_;
};

class FreeStack : public boost::noncopyable
{
	unsigned short ptr_;
	unsigned short stack_[MaxInstances];
public:
	FreeStack(): ptr_(0)
	{
		for(int i = 0; i < MaxInstances; ++i)
		{
			stack_[i] = i;
		}
	}

	unsigned short GetNextFreeInstance()
	{
		unsigned short result;
		if (ptr_ < MaxInstances)
		{
			result = stack_[ptr_];
			ptr_++;
		}
		else
		{
            throw FreeStackError(FreeStackError::INSTANCES_EXCEEDED);
		}
		return result;
	}

	// Free the instance and 
	void FreeInstance(unsigned short instance)
	{
		if (ptr_ == 0)
		{
            throw FreeStackError(FreeStackError::INSTANCES_LESS_THAN_ZERO);
		}
		else
		{
			ptr_--;
			stack_[ptr_] = instance;
		}
	}

#ifdef ADEBUG
    template <class T> void PrintFreeList(T& str)
    {
        for (int i = 0; i < MaxInstances; ++i)
        {
            str << stack_[i] << "\n";
        }
        str << "--------------\n";
    }
#endif
};

template <class T, typename I=unsigned short> class HasSmallInstanceNumber
{
	static I s_instanceNumber;
	static T* s_instances_[MaxInstances];
	I instanceNumber_;
	static FreeStack freestack_;
public:
	HasSmallInstanceNumber<T, I>():instanceNumber_(freestack_.GetNextFreeInstance()) {
        s_instances_[instanceNumber_] = static_cast<T*>(this);
    }
	~HasSmallInstanceNumber<T, I>() {freestack_.FreeInstance(instanceNumber_);}
	inline I GetInstanceNumber() {return instanceNumber_;}
	static T* GetInstanceFromInstanceNumber(I instanceNumber)
	{
		T* result = s_instances_[instanceNumber];
		return result;
	}
#ifdef ADEBUG  
    template <class T>static void PrintList(T& str)
    {
        freestack_.PrintFreeList(str);
    }
#endif

	I GetNextInstanceNumber() {};
};

// define statics:
template <class T, typename I> I HasSmallInstanceNumber<T, I>::s_instanceNumber = 0;
template <class T, typename I> T* HasSmallInstanceNumber<T, I>::s_instances_[MaxInstances] = {0};
template <class T, typename I> FreeStack HasSmallInstanceNumber<T, I>::freestack_;

} // namespace 
} // namespace ams
