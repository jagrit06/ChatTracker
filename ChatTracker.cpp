// ChatTracker.cpp

#include "ChatTracker.h"
#include <string>

#include <vector>
#include <list>
#include <forward_list>
#include <algorithm>

#include <functional>

using namespace std;

////////////////////////////////////////////////////////////////////////
// User and Chat struct
////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////
// User Declarations

// User has:
// 		m_name : name of the user
// 		m_chats: list of Chats and the asscoiated no. of contributions 
//				 made by the user to that Chat. Holds the Chats in an order 
//				 such that the Chat joined last by the User is in the front
// 				 Note: the current chat is given by front of m_chats
//
// User can:
//		be contructed with an empty chat list with only a name
//		join a chat given a pointer to the chat
//		leave a chat given a pointer to it
//		leave current chat
//		contribute to current chat

struct Chat;

struct User
{
	// Data Members
	string m_name;
	list< pair<Chat*, int> > m_chats;

	// Functions
	User(const string& name) : m_name(name) {}

	void joinChat(Chat* chat);
	int leaveChat(Chat* chat);

	int leaveCurrChat();
	int contributeToCurrChat();
};

////////////////////////////////////////////////////////////////////////
// Chat Declarations

// Chat has:
// 		m_name : name of the chat
// 		m_users: cumulative list of Users associated with that chat 
//		m_contributions: keeps track of the contributions made to a chat
//
// Chat can:
//		be contructed with an empty user list with only a name
//		add a user to the chat given a pointer to the user
// 		recieve contributions
//		terminate itself (make all users leave)

struct Chat
{
	// Data Members
	string m_name;
	forward_list<User*> m_users;
	int m_contributions;

	// Functions
	Chat(const string& name) : m_name(name), m_contributions(0) {}

	void addUser(User* user);
	void recContribution() { m_contributions++; }
	int terminateChat();
};

////////////////////////////////////////////////////////////////////////
// User Function Implementations

void User::joinChat(Chat* chat)
	// Makes chat user's current chat (bring to front of m_chats list)
	// If user not already associated with chat
	//		Add chat to front of m_chats
	//		Set contributions by user to chat to 0
	// If the user had previously joined that chat and not left it since then
	//		User's count of contributions to that chat remains the same
	//		Chat is made current chat
	//
	// Calls addUser from chat if needed 
	// !!!! Does not check for nullptr
{
	list< pair<Chat*, int> >::iterator p = m_chats.begin();

	for(; p!= m_chats.end(); p++)
	{
		if (p->first == chat)
		// If the user had previously joined that chat and not left
		{
			pair<Chat*, int> toMakeCurrentChat = *p;

			// Chat is brought to the front of the list
			m_chats.erase(p);
			m_chats.push_front(toMakeCurrentChat);

			// No need to add user to chat again
			return;
		}
	}

	// If user not already associated with chat

	// Add chat to front of m_chats and Set contributions to 0
	m_chats.push_front(make_pair(chat, 0));
	// Add user to chat
	chat->addUser(this);
}

int User::leaveChat(Chat* chat)
	// If the user is not associated wih the indicated chat, 
	//		return -1. 
	// Otherwise, 
	//		remove chat from m_chat 
	//		return the user's count of contributions  
	// This function leaves the chat's count of contributions unchanged.
	//!!!! Does not check for nullptr
{
	list< pair<Chat*, int> >::iterator p = m_chats.begin();
	// Perform linear to find chat in user
	for(; p!= m_chats.end(); p++)
	{
		if (p->first == chat)
		// If the user had previously joined that chat and not left
		{
			int contributions = p->second;
			// Remove chat 
			m_chats.erase(p);
			return contributions;
		}
	}
	// If user not associated with chat
	return -1;
}

int User::leaveCurrChat()
	// If the user has no current chat, return -1. 
	// Otherwise, 
	//		the user is no longer associated with that user's current chat
	// 		the user's current chat becomes the most recently joined chat 
	//		that hasn't been left
	//		return the user's count of contributions made to that chat. 
	// This function leaves the chat's count of contributions unchanged. 
{
	//If the user has no current chat
	if (m_chats.empty())
		return -1;
	
	int contributions = m_chats.begin()->second;
	// Remove current chat 
	m_chats.erase(m_chats.begin());
	return contributions;
}

int User::contributeToCurrChat()
	// If the user has no current chat, return 0. 
	// Otherwise, 
	//		the user's count of contributions to user's current chat and that
	//		current chat's count of contributions are increased by one.
	//		Return the user's new count of contributions to current chat. 
{
	// If no current chat
	if (m_chats.empty()) return 0;
	
	m_chats.begin()->first->recContribution();
	return ++m_chats.begin()->second;
}

////////////////////////////////////////////////////////////////////////
// Chat Function Implementations

void Chat::addUser(User* user)
	// Note user as a member of the chat
	// !!!!! Does not check for nullptr 
{
	m_users.insert_after(m_users.before_begin(), user);
}

int Chat::terminateChat()
	// All users that were associated with the chat leave
	// Return the total contributions made to the chat
{
	forward_list< User* >::iterator p = m_users.begin();
	for (; p != m_users.end(); p++)
	{
		(*p)->leaveChat(this);
	}
	return m_contributions;
}

////////////////////////////////////////////////////////////////////////
// Chat Tracker Implementation Class
////////////////////////////////////////////////////////////////////////

class ChatTrackerImpl
{
public:
	// Constructor and Destructor
	ChatTrackerImpl(int maxBuckets)
	: m_maxBuckets(maxBuckets), m_chats(maxBuckets), m_users(maxBuckets){}
	~ChatTrackerImpl();

	// Implementation Functions
	void join(const string& user, const string& chat);
	int terminate(const string& chat);
	int contribute(const string& user);
	int leave(const string& user, const string& chat);
	int leave(const string& user);

private:
	int m_maxBuckets;

	vector<list<Chat*>> m_chats;
	vector<list<User*>> m_users;

	// Hash Table Functions
	unsigned int mapFunc(const string& hashMe);
	Chat* findChat(const string& chatName);
	User* findUser(const string& userName);
};

/////////////////////////////////////////////////////////////////////////
// Destructor

ChatTrackerImpl::~ChatTrackerImpl()
{
	// Loop over all remaining chats and delete them
	for (int i = 0; i < m_chats.size(); i++)
	{
		if(m_chats[i].empty()) continue;
		for (list<Chat*>::iterator p = m_chats[i].begin(); p != m_chats[i].end(); p++)
		{
			delete *p;
		}
	}
	// Loop over all remaining users and delete them
	for (int i = 0; i < m_users.size(); i++)
	{
		if(m_users[i].empty()) continue;
		for (list<User*>::iterator p = m_users[i].begin(); p != m_users[i].end(); p++)
		{
			delete *p;
		}
	}
}

////////////////////////////////////////////////////////////////////////
// Hash Table Functions

unsigned int ChatTrackerImpl::mapFunc(const string& hashMe)
{
	hash<string> str_hash;
	return str_hash(hashMe) % m_maxBuckets;
}

Chat* ChatTrackerImpl::findChat(const string& chatName)
	// Find the bucket to which the chat belongs
	// Perform a linear search to get pointer to the chat
	// Return nullptr if no such chat 
{
	unsigned int bucket = mapFunc(chatName);
	list<Chat*>::iterator p = m_chats[bucket].begin();

	for (; p != m_chats[bucket].end(); p++)
	{
		if ((*p)->m_name == chatName)
			return *p;
	}
	return nullptr;
}

User* ChatTrackerImpl::findUser(const string& userName)
	// Find the bucket to which the user belongs
	// Perform a linear search to get pointer to the user
	// Return nullptr if no such user
{
	unsigned int bucket = mapFunc(userName);
	list<User*>::iterator p = m_users[bucket].begin();

	for (; p != m_users[bucket].end(); p++)
	{
		if ((*p)->m_name == userName)
			return *p;
	}

	return nullptr;
}

/////////////////////////////////////////////////////////////////////////
// Implementation Functions

void ChatTrackerImpl::join(const string& user, const string& chat)
	// If user or chat do not exist, make them
	// If user not already associated with chat
	//		Associate user to the chat 
	// If the user had previously joined that chat and not left it since then
	//		User's count of contributions to that chat remains the same
	//		Chat is brought to the front of the list
{
	Chat* chatToAdd = findChat(chat);
	if (!chatToAdd)  // If no such chat exists
	{
		chatToAdd = new Chat(chat);
		m_chats[mapFunc(chat)].push_back(chatToAdd);
	}

	User* userToAdd = findUser(user);
	if (!userToAdd)  // If no such user exists
	{
		userToAdd = new User(user);
		m_users[mapFunc(user)].push_back(userToAdd);
	}

	userToAdd->joinChat(chatToAdd);
}

int ChatTrackerImpl::terminate(const string& chat)
	// If chat has no users associated with it or does not exist,return 0
	// Otherwise, all users that were associated with the chat leave

	// If that chat was any user's current chat,
	//	then the existing chat the user most recently joined and hasn't left 
	//	since most recenty joining becomes the user's current chat; if there 
	//	is no such chat, then that user has no current chat. 
{
	Chat* toTerminate = findChat(chat);
	
	if (!toTerminate)  // If no such chat
		return 0;

	int contributions = toTerminate->terminateChat();
	
	// Remove chat from hash table
	m_chats[mapFunc(chat)].remove(toTerminate);

	delete toTerminate;

	// Return contributions
	return contributions;
}

int ChatTrackerImpl::contribute(const string& user)
	//If the user has no current chat, return 0. 
	// Otherwise, 
	//		the user's count of contributions to user's current chat and that
	//		current chat's count of contributions are increased by one.
	//		Return the user's new count of contributions to current chat. 
{
	User* toContribute = findUser(user);

	if (!toContribute) return 0;  // If no such user exists

	return toContribute->contributeToCurrChat();
}

int ChatTrackerImpl::leave(const string& user, const string& chat)
	// If the user is not associated wih the indicated chat, return -1. 
	// Otherwise, 
	//		remove chat from m_chat 
	//		return the user's count of contributions  
	//
	// If chat is current chat, then the existing chat the user most recently
	// joined and hasn't left since most recenty joining becomes the user's 
	// current chat; if there is no such chat, then user has no current chat. 
	//
	// This function leaves the chat's count of contributions unchanged.
{
	User* toLeave = findUser(user);
	Chat* chatToLeave = findChat(chat);

	if (!toLeave || !chatToLeave) return -1; // If no such user or chat exists

	return toLeave->leaveChat(chatToLeave);
}

int ChatTrackerImpl::leave(const string& user)
	// If the user has no current chat, return -1. 
	// Otherwise, 
	//		the user is no longer associated with that user's current chat
	// 		the user's current chat becomes the most recently joined chat 
	//		that hasn't been left
	//		return the user's count of contributions made to that chat. 
	// This function leaves the chat's count of contributions unchanged. 
{
	User* toLeave = findUser(user);

	if (!toLeave) return -1; // If no such user exists

	return toLeave->leaveCurrChat();
}

////////////////////////////////////////////////////////////////////////
//*********** ChatTracker functions **************
////////////////////////////////////////////////////////////////////////

// These functions simply delegate to ChatTrackerImpl's functions.

ChatTracker::ChatTracker(int maxBuckets)
{
	m_impl = new ChatTrackerImpl(maxBuckets);
}

ChatTracker::~ChatTracker()
{
	delete m_impl;
}

void ChatTracker::join(string user, string chat)
{
	m_impl->join(user, chat);
}

int ChatTracker::terminate(string chat)
{
	return m_impl->terminate(chat);
}

int ChatTracker::contribute(string user)
{
	return m_impl->contribute(user);
}

int ChatTracker::leave(string user, string chat)
{
	return m_impl->leave(user, chat);
}

int ChatTracker::leave(string user)
{
	return m_impl->leave(user);
}