#pragma once
#include "cdbpch.h"
#include "BlockingCollection.h"
#include "CadBerry/Core.h"

#include <thread>
#include <atomic>

namespace CDB
{
	class ThreadPool;
	class CDBAPI ThreadPool
	{
	public:
		static ThreadPool* Get();
		~ThreadPool();
		void CompleteTasksAndDelete();

		bool AddStandardTask(std::function<void()> Func);
		bool AddPriorityTask(std::function<void()> Func);

		bool AddBackgroundTask(std::function<void()> Func);
	private:
		ThreadPool();

		void MaintainThread();

		bool EndThreads = false;
		bool IsOpen = true;
		std::atomic<bool> IsWaiting = false;

		unsigned int NumAvailableThreads;

		static ThreadPool* Pool;

		code_machina::BlockingCollection<std::function<void()>> StandardTasks;
		code_machina::BlockingCollection<std::function<void()>> PriorityTasks;
		code_machina::BlockingCollection<std::function<void()>> BackgroundTasks;

		std::vector<std::thread> WorkerThreads;
	};
}