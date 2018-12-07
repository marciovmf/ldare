#ifndef _LDK_MODULE_H_
#define _LDK_MODULE_H_

namespace ldk
{
	struct Module
	{
		virtual ldkSTATUS setup() = 0;
		virtual ldkSTATUS initialize() = 0;	
		virtual void update() = 0;
		virtual void terminate() = 0;
	};

	struct EventQueueModule : Module
	{	
		virtual ldkSTATUS setup()
		{
			return LDK_STATUS_SUCCESS;
		}

		virtual ldkSTATUS initialize()
		{
			return LDK_STATUS_SUCCESS;
		}

		virtual void update()
		{
		}

		virtual void terminate()
		{
		}
	};

}
#endif // _LDK_MODULE_H_
