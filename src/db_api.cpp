
#include "bits/stdc++.h"
#include "include/db_api.h"

DBController::DBController() {
	// auto storage = make_storage("db.sqlite",
    //     make_table("users",
    //        make_column("id", &User::id, primary_key().autoincrement()),
    //        make_column("first_name", &User::firstName),
    //        make_column("last_name", &User::lastName),
    //        make_column("birth_date", &User::birthDate),
    //        make_column("image_url", &User::imageUrl),
    //        make_column("type_id", &User::typeId)),
    //     make_table("user_types",
    //        make_column("id", &UserType::id, primary_key().autoincrement()),
    //        make_column("name", &UserType::name, default_value("name_placeholder"))));

	auto storage = make_storage("db.sqlite",
        make_table("links",
           make_column("id", &Link::id, primary_key().autoincrement()),
           make_column("url", &Link::url),
           make_column("title", &Link::title),
           make_column("visited_time", &Link::visited_time)
        )
    );
}