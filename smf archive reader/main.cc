#include "minijson_reader.hh" // https://github.com/giacomodrago/minijson_reader
#include <string_view>

#include <fstream>
#include <string>
#include <vector>
#include <map>

#include <iostream>
#include <chrono>
#include <ctime>


/* -------------- */
/*  Define Types  */
/* -------------- */

struct Forum;
struct Topic;
struct Post;
struct Body;
using UnixTime = unsigned long long;
using BoardIndex = std::vector<std::string>;

struct Forum {
	std::string name;
	std::string url;
	std::vector<Topic> topics;	
};

struct Topic {
	std::size_t id;
	std::string url;
	BoardIndex boardIndex;
	std::vector<Post> posts;
};

struct Post {
	std::size_t id;
	std::string url;
	std::string title;
	std::string user;
	UnixTime date;
	std::string body;

	std::size_t parentTopicID;
};



/* -------------- */
/* Read Functions */
/* -------------- */

class ArchivedForum : public Forum {
private:
	using TopicIndex = std::array<std::size_t,1>; // internal index values
	using PostIndex  = std::array<std::size_t,2>; // (different from site id's)
	std::map<std::size_t, TopicIndex> topicMap = {};
	std::map<std::size_t, PostIndex > postMap  = {};

public:
	bool   hasTopic(std::size_t topicID) { return topicMap.contains(topicID); }
	Topic& getTopic(std::size_t topicID) {
		auto ids = topicMap.at(topicID);
		return this->topics[ids[0]];
	}

	bool  hasPost(std::size_t postID) { return postMap.contains(postID); }
	Post& getPost(std::size_t postID) {
		auto ids = postMap.at(postID);
		return this->topics[ids[0]].posts[ids[1]];
	}

	std::size_t numTopics() { return topicMap.size(); }
	std::size_t numPosts () { return postMap .size(); }

	void iterateTopics(auto&& f) {
		for (const auto& [topicID, _ ] : topicMap) {
			if constexpr (std::is_invocable_v<decltype(f), std::size_t, Topic&>)
			     f(topicID, getTopic(topicID));
			else f(/*    */ getTopic(topicID));
	} }

	void iteratePosts(auto&& f) {
		for (const auto& [postID, _ ] : postMap) {
			if constexpr (std::is_invocable_v<decltype(f), std::size_t, Post&>)
			     f(postID, getPost(postID));
			else f(/*   */ getPost(postID));
	} }

	void storeTopicIndex(const Topic& topic, TopicIndex ids) { topicMap[topic.id] = ids; }
	void storePostIndex (const Post & post , PostIndex  ids) { postMap [post .id] = ids; }
};



/* -------------- */
/*      Misc      */
/* -------------- */

#define Error(STR) (std::cout << "ERROR:\n"<< (STR) << "\n\n" , \
                    std::exit(__LINE__))

namespace minijson {
	template<> struct value_as<std::string> {
		std::string operator() (const value v) {
			return std::string{
				value_as_default<std::string_view>(v)};
		}
	};
}

std::ostream& operator<<(std::ostream& os, const BoardIndex& boardIndex) {
	os << "[ ";
	for (std::size_t i=0; i < boardIndex.size(); i++) {
		os << boardIndex[i];
		if (i+1 < boardIndex.size()) { os << " | "; }
	}
	os << " ]";
	return os;
}

std::string dateToString(UnixTime inDate) {
	const std::time_t date = inDate;
	return std::string{std::ctime(&date), 26};
}

// I like to have print functions near EOF :P
void printTopic(ArchivedForum&, std::size_t TopicID);
void printPost (ArchivedForum&, std::size_t PostID);
void printHelp ();



/* -------------- */
/*     Macros     */
/* -------------- */

#define Assert_Array()  if (val.type() != minijson::Array ) { Error("Array expected"); }
#define Assert_Object() if (val.type() != minijson::Object) { Error("Object expected"); }
#define Assert_Type(TYPE) if (val.as<std::string>() != TYPE) { Error(TYPE " object expected"); }

#define TopLevel_Array()      minijson::parse_array (context, [&](Value val) {
#define TopLevel_Object(TYPE) minijson::parse_object(context, [&](std::string_view key, Value val) { \
                              if (key == "type") { Assert_Type(TYPE); }

#define Nested_Array()      Assert_Array();  minijson::parse_array (context, [&](Value val) {
#define Nested_Object(TYPE) Assert_Object(); minijson::parse_object(context, [&](std::string_view key, Value val) { \
                            if (key == "type") { Assert_Type(TYPE); }

#define Key(KEY) else if (key == KEY)
#define Store(VAR); VAR = val.as<decltype(VAR)>()

#define End });



/* -------------- */
/* Args Structure */
/* -------------- */

struct parseArguments {
	std::string fileName = "archive.json";
	enum {
		EXPLORE, POSTS, TOPICS ,
		POST_NAMES, TOPIC_NAMES , 
		TOPICS_AND_POSTS
	} runType = EXPLORE;

	parseArguments(int argc, const char* args[]) {
		if (argc > 1) { 
			std::string s{args[1]};
			if (s=="-h" || s=="-H" || s=="-help") { printHelp(); std::exit(0); }
			else if (runMap.contains(s)) { runType = runMap.at(s); }
			else { fileName = args[1]; }
		}
		if (argc > 2 && runType == EXPLORE) { 
			std::string s{args[2]};
			if (runMap.contains(s)) { runType = runMap.at(s); }
		}
	}
private:
	const std::map<std::string, decltype(runType)> runMap {
		{"-posts"           , POSTS           } ,
		{"-topics"          , TOPICS          } ,
		{"-post-names"      , POST_NAMES      } ,
		{"-topic-names"     , TOPIC_NAMES     } ,
		{"-topics-and-posts", TOPICS_AND_POSTS}
	};
};



/* -------------- */
/*  Main Program  */
/* -------------- */

int main(int argc, const char* args[]) {
	auto settings = parseArguments{argc, args};
	// open the .json archive
	std::ifstream fileInput{settings.fileName, std::ios::binary};
	if (!fileInput.is_open()) {
		Error("Failed to open Archive: " + settings.fileName);
	}
	// results in context for minijson
	minijson::istream_context context{fileInput};

	const auto parseTimeStart = std::chrono::steady_clock::now();

	// create forum container, and populate it!
	ArchivedForum forumData;
	using Value = minijson::value;

	TopLevel_Object("forum")
		Key("name") { Store(forumData.name); }
		Key("url" ) { Store(forumData.url);  }
		Key("topics") {
			Nested_Array()
				Topic topic = {};
				Nested_Object("topic")
					Key("id" ) { Store(topic.id);  }
					Key("url") { Store(topic.url); }
					Key("board-index") {
						Nested_Array()
							std::string boardName; Store(boardName);
							topic.boardIndex.emplace_back(boardName);
						End
					}
					Key("posts") {
						Nested_Array()
							Post post = {};
							Nested_Object("post")
								Key("id"   ) { Store(post.id);    }
								Key("url"  ) { Store(post.url);   }
								Key("title") { Store(post.title); }
								Key("user" ) { Store(post.user);  }
								Key("date" ) { Store(post.date);  }
								Key("body" ) { Store(post.body);  }
							End
							post.parentTopicID = topic.id;
							forumData.storePostIndex(post, {forumData.topics.size(), topic.posts.size()});
							topic.posts.push_back(std::move(post));
						End
					}
				End
				forumData.storeTopicIndex(topic, {forumData.topics.size()});
				forumData.topics.push_back(std::move(topic));
			End
		}
	End

	const auto parseTimeEnd = std::chrono::steady_clock::now();

	using milliseconds	 = std::chrono::milliseconds	;
	const auto parseTime = std::chrono::duration_cast<milliseconds>(parseTimeEnd - parseTimeStart);



	switch (settings.runType) {
	case settings.TOPIC_NAMES: {

		forumData.iterateTopics([](const Topic& topic) {
			std::cout << topic.posts[0].title << "\n";
		});
		} break;

	case settings.POST_NAMES: {

		forumData.iteratePosts([](const Post& post) {
			std::cout << post.id << " " << post.user << "\n";
		});
		} break;

	case settings.TOPICS: {

		forumData.iterateTopics([](const Topic& topic) {
			std::cout << topic.id << "\t";
			const Post& header = topic.posts[0];
			std::cout << header.title << "\t";
			std::cout << header.user << "\t";
			std::cout << header.date << "\t";
			std::cout << topic.url << "\t";
			/*for (std::size_t i=0; i < topic.posts.size(); i++) {
				std::cout << topic.posts[i].id;
				std::cout << ((i+1 < topic.posts.size()) ? ", " : "");
			}*/
			std::cout << topic.posts.size()-1;
			std::cout << "\n";
		});
		} break;

	case settings.POSTS: {

		forumData.iteratePosts([](const Post& post) {
			std::cout << post.id << "\t";
			std::cout << post.user << "\t";
			std::cout << post.date << "\t";
			std::cout << post.url << "\t";
			// std::cout << post.body;
			std::cout << "\n";
		});
		} break;

	case settings.TOPICS_AND_POSTS: {

		forumData.iterateTopics([](const Topic& topic) {
			const Post& header = topic.posts[0];
			std::cout << header.title << "\tby: " << header.user << "\n";
			for (std::size_t i=0; i < topic.posts.size(); i++) {
				const Post& post = topic.posts[i];
				std::cout << "\t" << post.user << " " << dateToString(post.date)
				          << "\t" << post.url << "\n";
			}
		});
		} break;

	case settings.EXPLORE: {

		std::cout << "Archive fully parsed! (in " <<  parseTime.count() << " milliseconds)\n\n"
		          << forumData.name << " has:\n"
		          << "\t" << forumData.numTopics() << "\ttopics,\n"
		          << "\t" << forumData.numPosts()  << "\tposts\n\n";

		char option;
		while (option != 'q' && option != 'Q') {
			std::cout << "[Type an option]\n";
			std::cin >> option;
			switch (option) {
			case 't': case 'T': {
				std::size_t topicID;
				std::cout << "[Enter Topic Number]: ";
				std::cin  >> topicID;
				printTopic(forumData, topicID);
				} break;
			case 'p': case 'P': {
				std::size_t postID;
				std::cout << "[Enter Post Number]: ";
				std::cin >> postID;
				printPost(forumData, postID);
				} break;
			}
		}
		} break;

	}

	return 0;
}

void printTopic(ArchivedForum& forumData, std::size_t topicID) {
	if (forumData.hasTopic(topicID)) {
		const Topic& topic = forumData.getTopic(topicID);
		const Post& header = topic.posts[0];

		std::cout << "Topic #" << topic.id << ":\t" << header.title << "\tby " << header.user << "\tat " << dateToString(header.date) << "\n"
		          << "\t" << topic.boardIndex << "\n"
		          << "\t" << topic.posts.size() << " posts:\n";

		for (std::size_t i=0; i < topic.posts.size(); i++) {
			std::cout << topic.posts[i].id;
			if (i+1 < topic.posts.size()) { std::cout << " "; }
		}
		std::cout << "\n";
	} else {
		std::cout << "Topic #" << topicID << " does not exist, or has been deleted.\n";
	}
}

void printPost(ArchivedForum& forumData, std::size_t postID) {
	if (forumData.hasPost(postID)) {
		const Post& post = forumData.getPost(postID);
		const Topic& topic = forumData.getTopic(post.parentTopicID);

		std::cout << "Post #" << post.id << "\tby " << post.user << "\tin "<< topic.posts[0].title << "\tat " << dateToString(post.date) << "\n"
		          << post.body << "\n";
	} else {
		std::cout << "Post #" << postID << " does not exist, or has been deleted.\n";
	}
}

void printHelp() {
	std::cout << "args: [? json file] [? options]\n\n";
	std::cout << "-posts           :\tPrint id, username, date, & url as TSV\n"
	          << "-topics          :\tPrint id, title, user, date, url, & post count as TSV\n"
	          << "-post-names      :\tPrint new-line separated post names\n"
	          << "-topic-names     :\tPrint new-line separated topic names\n"
	          << "-topics-and-posts:\tPrint all topics with all reply titles (no id's)\n";
}