#pragma once
#include "cdbpch.h"
#include "BlockingCollection.h"
#include "CadBerry/Core.h"

#include <thread>
#include <atomic>
#include <future>

namespace CDB
{
	struct FuncIO
	{
		void* Params;
		std::promise<void*>* Returnpromise;
	};

	class ThreadPool;
	class CDBAPI ThreadPool
	{
	public:
		static ThreadPool* Get();
		~ThreadPool();
		void CompleteTasksAndDelete();

		std::pair<bool, std::future<void*>> AddStandardTask(std::function<void* (void*)> Func, void* Params);
		std::pair<bool, std::future<void*>> AddPriorityTask(std::function<void* (void*)> Func, void* Params);

		std::pair<bool, std::future<void*>> AddBackgroundTask(std::function<void* (void*)> Func, void* Params);
	private:
		ThreadPool();

		void MaintainThread();

		bool EndThreads = false;
		bool IsOpen = true;
		std::atomic<bool> IsWaiting = false;

		unsigned int NumAvailableThreads;

		static ThreadPool* Pool;

		code_machina::BlockingCollection<std::pair<FuncIO, std::function<void* (void*)>>> StandardTasks;
		code_machina::BlockingCollection<std::pair<FuncIO, std::function<void* (void*)>>> PriorityTasks;
		code_machina::BlockingCollection<std::pair<FuncIO, std::function<void* (void*)>>> BackgroundTasks;

		std::vector<std::thread> WorkerThreads;
	};
}