#include "cdbpch.h"
#include "ThreadPool.h"
#include "CadBerry/Log.h"

#include <chrono>

namespace CDB
{
	ThreadPool* ThreadPool::Pool = nullptr;

	ThreadPool* ThreadPool::Get()
	{
		if (Pool == nullptr)
		{
			Pool = new ThreadPool();
		}
		return Pool;
	}

	ThreadPool::ThreadPool()
	{
		NumAvailableThreads = std::thread::hardware_concurrency();
		int NumWorkerThreads = 1;
		if (NumAvailableThreads == 0)
		{
			NumAvailableThreads = 1;    //We know there's at least one thread
		}
		else if (NumAvailableThreads > 2)
		{
			NumWorkerThreads = NumAvailableThreads - 2;
		}

		CDB_BuildInfo("Creating thread pool of {0} threads out of a total of {1} threads", NumWorkerThreads, NumAvailableThreads);
		WorkerThreads.reserve(NumWorkerThreads);

		for (int i = 0; i < NumWorkerThreads; ++i)    //Create the worker threads
		{
			WorkerThreads.push_back(std::thread(&ThreadPool::MaintainThread, this));
		}
	}

	ThreadPool::~ThreadPool()
	{
		EndThreads = true;
		for (std::thread& thread : WorkerThreads)
		{
			thread.join();
		}
		Pool = nullptr;
	}

	void ThreadPool::CompleteTasksAndDelete()
	{
		CDB_EditorInfo("Waiting for tasks to finish");
		IsOpen = false;
		while (PriorityTasks.size() > 0 && StandardTasks.size() > 0)
		{
			Sleep(100);
		}
		CDB_EditorInfo("All tasks finished. Freeing threads");
		delete this;
	}
	
	bool ThreadPool::AddStandardTask(std::function<void()> Func)
	{
		if (IsOpen)
		{
			StandardTasks.add(Func);
			return true;
		}
		return false;
	}
	
	bool ThreadPool::AddPriorityTask(std::function<void()> Func)
	{
		if (IsOpen)
		{
			PriorityTasks.add(Func);
			return true;
		}
		return false;
	}
	
	bool ThreadPool::AddBackgroundTask(std::function<void()> Func)
	{
		if (IsOpen)
		{
			BackgroundTasks.add(Func);
			return true;
		}
		return false;
	}

	void ThreadPool::MaintainThread()
	{
		while (!EndThreads)
		{
			std::function<void()> ToBeExecuted;
			auto Success = PriorityTasks.try_take(ToBeExecuted);    //Check if there is a priority task, these will be executed first

			if (Success == code_machina::BlockingCollectionStatus::Ok)
			{
				ToBeExecuted();
				continue;
			}
			Success = StandardTasks.try_take(ToBeExecuted, std::chrono::milliseconds(50));    //Check if there is a standard task

			if (Success == code_machina::BlockingCollectionStatus::Ok)
			{
				ToBeExecuted();
				continue;
			}

			if (!IsOpen)    //If there aren't any priority or standard tasks and the thread pool isn't accepting new tasks, return
			{
				return;
			}
			if (!IsWaiting)    //If another thread isn't already waiting for new tasks
			{
				IsWaiting = true;    //Tell the other threads that this thread is waiting
				Success = PriorityTasks.try_take(ToBeExecuted, std::chrono::milliseconds(25));
				if (Success == code_machina::BlockingCollectionStatus::Ok)
				{
					ToBeExecuted();
					continue;
				}

				Success = StandardTasks.try_take(ToBeExecuted, std::chrono::milliseconds(25));
				if (Success == code_machina::BlockingCollectionStatus::Ok)
				{
					ToBeExecuted();
					continue;
				}
				IsWaiting = false;
				continue;
			}

			Success = BackgroundTasks.try_take(ToBeExecuted, std::chrono::milliseconds(150));
			if (Success == code_machina::BlockingCollectionStatus::Ok)
			{
				ToBeExecuted();
			}
		}
	}
}
