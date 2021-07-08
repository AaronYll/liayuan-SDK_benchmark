// azqueue-test-000.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <Windows.h>
#include <iostream>
#include <was/storage_account.h>
#include <was/queue.h>
#include <time.h>
#include <numeric>
#include <string>
using std::vector;
using std::string;

typedef vector<azure::storage::cloud_queue_message> *messages;
typedef vector<long> *lvector;
typedef vector<std::wstring> *queues;
#define TEST_QUEUE_SIZE 80	// one queue contains 256 message(task)

int main()
{
	// Define the connection-string
	const utility::string_t storage_connection_string(U("DefaultEndpointsProtocol=https;AccountName=YOURACCOUNTNAME;AccountKey=YOURACCONTKEY"));
	//const utility::string_t storage_connection_string(U("UseDevelopmentStorage=true;"));

	// Retrieve storage account from connection string.
	azure::storage::cloud_storage_account storage_account = azure::storage::cloud_storage_account::parse(storage_connection_string);

	// Create a queue client
	azure::storage::cloud_queue_client q_client = storage_account.create_cloud_queue_client();
	queues queue_list = new vector<std::wstring>();
	// Retrieve a reference to a queue and create the queue if it doesn't already exist
	
	for (int j = 0; j < TEST_QUEUE_SIZE; j++) {
		string id = "otestaccount1batchtest10-a-0d7f284d-d06c-4d56-9c42-69d2f7f9ab" + std::to_string(j);
		std::wstring queue_id(id.begin(), id.end());
		//utility::string_t queue_id = std::to_string("testaccount1batchtest10-a-0d7f284d-d06c-4d56-9c42-69d2f7f9ab") + std::to_string(j));
		queue_list->push_back(queue_id);
		std::wcout << "Create new message queue: " << queue_id << std::endl;
		azure::storage::cloud_queue queue = q_client.get_queue_reference(queue_id);
		queue.create_if_not_exists();

		azure::storage::cloud_queue_message message(U("<JobQueueEntry><AccountName>testaccount1batchtest10 22F0F409DBB32A7A</AccountName><WorkitemName>abs-0-job18285-0-p1 22F0F350A3B8F1B6</WorkitemName><JobName>job-1</JobName><TaskName>abs-0-job18285-0-p1-task-8</TaskName><TaskGuid>c19522e9-c3f3-408a-aff6-77f3838609fb</TaskGuid><InsertionTime>132688024644038415</InsertionTime><ActivityId>b7085eef-1a09-444b-8f57-4c3c89a673e3</ActivityId><NumInstances>1</NumInstances><RequiredSlots>1</RequiredSlots></JobQueueEntry>"));
		// insert messages into the queue(256 messages)
		for (int i = 0; i < 256; i++)
		{
			queue.add_message(message);
		}
		std::wcout << j+1 << "/" << TEST_QUEUE_SIZE << "       256 messages insert into queue: " << queue_id << "\n" <<std::endl;
	}

	std::cout << "\n ---------------Insert message finish, sleep 30 seconds---------------" << std::endl;
	Sleep(30000);
		
	// Dequeue some queue messages (maximum 32 at a time) and set their visibility timeout.
	azure::storage::queue_request_options options;
	azure::storage::operation_context context;
	vector<long> *consume_times = new vector<long>();
	vector<int> *message_nums = new vector<int>();

	// Get messages
	for (int i = 0; i < TEST_QUEUE_SIZE; i++)
	{
		std::wstring queue_id = (*queue_list)[i];
		std::wcout << "Start to pull message from: " << queue_id << std::endl;
		long patch_start, patch_end;
		azure::storage::cloud_queue queue = q_client.get_queue_reference(queue_id);

		for (int j = 0; j < 2; j++) 
		{
			long time_cnt = 0;
			long message_cnt = 0;

			for (int k = 0; k < 4; k++)
			{
				patch_start = clock();
				vector<azure::storage::cloud_queue_message> group_messages = queue.get_messages(32, std::chrono::seconds(180), options, context);
				patch_end = clock();

				long time_consumed = patch_end - patch_start;
				long group_msg_sz = group_messages.size();
				time_cnt += time_consumed;
				message_cnt += group_msg_sz;
				consume_times->push_back(time_consumed);
				message_nums->push_back(group_msg_sz);
			}
			consume_times->push_back(time_cnt);
			message_nums->push_back(message_cnt);
		}
		Sleep(10000);	// sleep 10 second to simulate submit job manually
	}

	std::cout << "\n ---------------Start to show time info---------------" << std::endl;
	for (int i = 0; i < consume_times->size(); i++)
	{
		if (i % 5 == 0)	std::cout << "batch " << i/5+1 << ": [" << (*consume_times)[i];
		else if (i % 5 == 3) std::cout << ", " << (*consume_times)[i] << "]";
		else if (i % 5 == 4) std::cout << ",   total time: " << (*consume_times)[i] << std::endl;
		else std::cout << ", " << (*consume_times)[i];
	}

	std::cout << "\n ---------------Start to show message number info:---------------" << std::endl;
	for (int i = 0; i < message_nums->size(); i++)
	{
		if (i % 5 == 0)	std::cout << "batch " << i / 5 + 1 << ": [" << (*message_nums)[i];
		else if (i % 5 == 3) std::cout << ", " << (*message_nums)[i] << "]";
		else if (i % 5 == 4) std::cout << ",   total messages: " << (*message_nums)[i] << std::endl;
		else std::cout << ", " << (*message_nums)[i];
	}
	
	std::cout << "\n ---------------Cleaning the resource...---------------" << std::endl;
	for (int i = 0; i < TEST_QUEUE_SIZE; i++)
	{
		std::wstring queue_id = (*queue_list)[i];
		azure::storage::cloud_queue queue = q_client.get_queue_reference(queue_id);
		queue.delete_queue_if_exists();
	}
	std::cout << "\n ---------------Cleaning resource finished---------------" << std::endl;
	system("pause");
}
