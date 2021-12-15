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

		/*
		BackgroundTasks is a list of functions that threads can do while they're sitting idle. This isn't thread-safe, but that doesn't really
		matter. It's fine if two threads are running the same function at the same time, since that's what they're for. One thread will always 
		be kept free. 
		*/
		std::vector<std::thread> WorkerThreads;
	};
}