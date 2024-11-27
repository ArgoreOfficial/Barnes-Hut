#pragma once

#include <thread>
#include <mutex>
#include <vector>

class cNode;
class sPoint;
class cOctree;

class cThreadPool
{
public:
	struct sWorker
	{
		bool busy = false;
		bool running = false;

		std::vector<sPoint*> point_queue;

		std::thread thread;
		std::mutex mutex;

		cThreadPool* thread_pool;

		double delta_time;

		cOctree* octree = nullptr;
	};

	 cThreadPool();
	~cThreadPool();

	void createWorkers( unsigned int _count, cOctree* _octree );
	bool doneWorking() { return m_busy_workers == 0; }
	void queueWork( sPoint* _point, double _delta_time );

	void threadFinished();

private:
	std::mutex m_mutex;
	std::vector<sWorker*> m_workers;
	int m_last_worked = 0;
	int m_busy_workers = 0;
};