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
	
	std::pair<bool, std::future<void*>> ThreadPool::AddStandardTask(std::function<void* (void*)> Func, void* Params)
	{
		if (IsOpen)
		{
			Func = std::bind(Func, Params);
			auto p = new std::promise<void*>();
			StandardTasks.add({ {Params, p}, Func });
			return { true, p->get_future() };
		}
		return { false, std::future<void*>() };
	}
	
	std::pair<bool, std::future<void*>> ThreadPool::AddPriorityTask(std::function<void* (void*)> Func, void* Params)
	{
		if (IsOpen)
		{
			Func = std::bind(Func, Params);
			auto p = new std::promise<void*>();
			PriorityTasks.add({ {Params, p}, Func });
			return { true, p->get_future() };
		}
		return { false, std::future<void*>() };
	}
	
	std::pair<bool, std::future<void*>> ThreadPool::AddBackgroundTask(std::function<void* (void*)> Func, void* Params)
	{
		if (IsOpen)
		{
			Func = std::bind(Func, Params);
			auto p = new std::promise<void*>();
			BackgroundTasks.add({ {Params, p}, Func });
			return { true, p->get_future() };
		}
		return { false, std::future<void*>() };
	}

	//TODO: Refactor this (maybe by writing my own promise class) so that I don't have to use pointers
	void ThreadPool::MaintainThread()
	{
		std::promise<void*> Placeholderpromise;
		std::pair<FuncIO, std::function<void* (void*)>> ToBeExecuted = { {nullptr, nullptr}, {} };
		while (!EndThreads)
		{
			auto Success = PriorityTasks.try_take(ToBeExecuted);    //Check if there is a priority task, these will be executed first

			if (Success == code_machina::BlockingCollectionStatus::Ok)
			{
				ToBeExecuted.first.Returnpromise->set_value(ToBeExecuted.second(ToBeExecuted.first.Params));
				delete ToBeExecuted.first.Returnpromise;
				continue;
			}
			Success = StandardTasks.try_take(ToBeExecuted, std::chrono::milliseconds(50));    //Check if there is a standard task

			if (Success == code_machina::BlockingCollectionStatus::Ok)
			{
				ToBeExecuted.first.Returnpromise->set_value(ToBeExecuted.second(ToBeExecuted.first.Params));
				delete ToBeExecuted.first.Returnpromise;
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
					ToBeExecuted.first.Returnpromise->set_value(ToBeExecuted.second(ToBeExecuted.first.Params));
					delete ToBeExecuted.first.Returnpromise;
					continue;
				}

				Success = StandardTasks.try_take(ToBeExecuted, std::chrono::milliseconds(25));
				if (Success == code_machina::BlockingCollectionStatus::Ok)
				{
					ToBeExecuted.first.Returnpromise->set_value(ToBeExecuted.second(ToBeExecuted.first.Params));
					delete ToBeExecuted.first.Returnpromise;
					continue;
				}
				IsWaiting = false;
				continue;
			}

			Success = BackgroundTasks.try_take(ToBeExecuted, std::chrono::milliseconds(150));
			if (Success == code_machina::BlockingCollectionStatus::Ok)
			{
				ToBeExecuted.first.Returnpromise->set_value(ToBeExecuted.second(ToBeExecuted.first.Params));
				delete ToBeExecuted.first.Returnpromise;
			}
		}
	}
}
