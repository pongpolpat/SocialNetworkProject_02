#include <iostream>
#include <fstream>
#include <unordered_map>
#include <set>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>

using namespace std;

class SocialNetwork {
private:
    unordered_map<string,string> users;          // username -> studentID
    unordered_map<string,set<string>> adj;       // adjacency list
    string currentUser;

    // ---------- Utility ----------
    static bool isNumber(const string &s){
        return !s.empty() && all_of(s.begin(), s.end(), ::isdigit);
    }

    vector<string> getAllUsers() {
        vector<string> v;
        for (auto &p : users) v.push_back(p.first);
        return v;
    }

    int degreeOf(const string &u){
        if(adj.count(u)) return (int)adj[u].size();
        return 0;
    }

    // คืนรายชื่อ user ที่ไม่ใช่ตัวเองและยังไม่ได้เป็นเพื่อน
    vector<string> nonFriendsPool(const string &u){
        vector<string> pool;
        for(auto &p : users){
            if(p.first != u && !adj[u].count(p.first))
                pool.push_back(p.first);
        }
        return pool;
    }

    // คืนเพื่อนของเพื่อนที่ยังไม่ได้เป็นเพื่อน (unique)
    vector<string> friendsOfFriends(const string &u){
        set<string> s;
        for(auto &f : adj[u]){
            for(auto &ff : adj[f]){
                if(ff != u && !adj[u].count(ff))
                    s.insert(ff);
            }
        }
        return vector<string>(s.begin(), s.end());
    }

    void pressBack(){
        cout << "\nPress 0 to Back: ";
        int x; cin >> x;
    }

public:

    // ================= LOAD / SAVE =================
    void loadUsers(){
        ifstream file("user.txt");
        string u,id;
        while(file >> u >> id){
            users[u] = id;
            adj[u]; // ensure node exists
        }
        file.close();
    }

    void loadFriends(){
        ifstream file("friend.txt");
        string u,v;
        while(file >> u >> v){
            adj[u].insert(v);
            adj[v].insert(u);
        }
        file.close();
    }

    void saveUsers(){
        ofstream file("user.txt");
        for(auto &p : users)
            file << p.first << " " << p.second << endl;
        file.close();
    }

    void saveFriends(){
        ofstream file("friend.txt");
        for(auto &p : adj){
            for(auto &f : p.second){
                if(p.first < f) // เก็บครั้งเดียว
                    file << p.first << " " << f << endl;
            }
        }
        file.close();
    }

    // ================= ADMIN =================
    bool adminLogin(){
        string pass;
        int attempt = 0;
        while(attempt < 3){
            cout << "Enter admin password: ";
            cin >> pass;
            if(pass == "JaRnyuo0075") return true;
            attempt++;
            cout << "Wrong password!\n";
        }
        cout << "Too many attempts.\n";
        return false;
    }

    void showAllUsers(){
        // เรียง user ตาม degree มาก -> น้อย
        vector<string> list = getAllUsers();
        sort(list.begin(), list.end(), [&](string a, string b){
            return degreeOf(a) > degreeOf(b);
        });

        cout << "\n===== SOCIAL NETWORK GRAPH =====\n\n";
        for(auto &u : list){
            cout << u << " (" << degreeOf(u) << ") : ";
            // เรียงเพื่อนตาม degree มาก -> น้อย
            vector<string> friends(adj[u].begin(), adj[u].end());
            sort(friends.begin(), friends.end(), [&](string a, string b){
                return degreeOf(a) > degreeOf(b);
            });
            for(int i=0;i<friends.size();i++){
                cout << friends[i];
                if(i != friends.size()-1) cout << ", ";
            }
            cout << endl;
        }
        cout << "\n================================\n";
        pressBack();
    }

    void addUser(){
        string u,id;
        cout << "Enter username: ";
        cin >> u;
        if(users.count(u)){
            cout << "Username exists.\n";
            return;
        }
        cout << "Enter student ID: ";
        cin >> id;
        if(!isNumber(id)){
            cout << "Student ID must be numeric.\n";
            return;
        }
        for(auto &p : users){
            if(p.second == id){
                cout << "Student ID exists.\n";
                return;
            }
        }
        users[u] = id;
        adj[u];
        saveUsers();
        cout << "User added.\n";
    }

    void removeUser(){
        string key;
        cout << "Search by 1 character: ";
        cin >> key;

        vector<string> result;
        for(auto &p : users){
            if(p.first.find(key) != string::npos)
                result.push_back(p.first);
        }
        if(result.empty()){
            cout << "No user found.\n";
            return;
        }

        for(int i=0;i<result.size();i++)
            cout << i+1 << ". " << result[i] << endl;

        int ch;
        cout << "Select: ";
        cin >> ch;
        if(ch < 1 || ch > result.size()) return;

        string delUser = result[ch-1];
        cout << "Confirm delete? (Y/N): ";
        char c; cin >> c;
        if(c!='Y' && c!='y') return;

        ofstream old("old_user.txt", ios::app);
        old << delUser << " " << users[delUser] << endl;
        old.close();

        users.erase(delUser);
        adj.erase(delUser);
        for(auto &p : adj)
            p.second.erase(delUser);

        saveUsers();
        saveFriends();
        cout << "User removed.\n";
    }

    void adminMenu(){
        while(true){
            cout << "\n===== ADMIN MENU =====\n";
            cout << "1. Show All Users\n";
            cout << "2. Add User\n";
            cout << "3. Remove User\n";
            cout << "0. Back\n";
            cout << "Choice: ";
            int ch; cin >> ch;

            if(ch==1) showAllUsers();
            else if(ch==2) addUser();
            else if(ch==3) removeUser();
            else if(ch==0) break;
        }
    }

    // ================= USER =================
    void registerUser(){
        string u,id;
        cout << "Enter username: ";
        cin >> u;
        if(users.count(u)){
            cout << "Username exists.\n";
            return;
        }
        cout << "Enter student ID: ";
        cin >> id;
        if(!isNumber(id)){
            cout << "Student ID must be numeric.\n";
            return;
        }
        for(auto &p : users){
            if(p.second == id){
                cout << "Student ID exists.\n";
                return;
            }
        }
        users[u] = id;
        adj[u];
        saveUsers();
        cout << "Registered.\n";
    }

    bool login(){
        string u,id;
        cout << "Username: ";
        cin >> u;
        cout << "Student ID: ";
        cin >> id;

        if(users.count(u) && users[u]==id){
            currentUser = u;
            return true;
        }
        cout << "Login failed.\n";
        return false;
    }

    void showAndAddFriend(){
        cout << "\n===== YOUR FRIENDS =====\n";

        if(adj[currentUser].empty()){
            cout << "No friends. Random 5 users:\n";
            auto pool = nonFriendsPool(currentUser);
            random_shuffle(pool.begin(), pool.end());
            for(int i=0;i<pool.size() && i<5;i++)
                cout << "- " << pool[i] << endl;
        } else {
            for(auto &f : adj[currentUser]){
                cout << "- " << f << endl;

                auto fof = friendsOfFriends(currentUser);
                cout << "  You may know:\n";
                int cnt = 0;
                for(auto &cand : fof){
                    if(cnt==3) break;
                    cout << "    " << cand << endl;
                    cnt++;
                }
            }

            cout << "\nRandom 2 users:\n";
            auto pool = nonFriendsPool(currentUser);
            random_shuffle(pool.begin(), pool.end());
            for(int i=0;i<pool.size() && i<2;i++)
                cout << "- " << pool[i] << endl;
        }

        cout << "\n1. Suggest Friend\n";
        cout << "2. Search Friend\n";
        cout << "0. Back\n";
        cout << "Choice: ";
        int ch; cin >> ch;

        if(ch==1){
            auto fof = friendsOfFriends(currentUser);
            auto pool = nonFriendsPool(currentUser);

            // รวมและไม่ซ้ำ
            set<string> s(fof.begin(), fof.end());
            for(auto &p : pool) s.insert(p);

            vector<string> suggest(s.begin(), s.end());
            cout << "\nSuggest 5:\n";
            for(int i=0;i<suggest.size() && i<5;i++)
                cout << i+1 << ". " << suggest[i] << endl;

            cout << "Select to add (0 cancel): ";
            int c; cin >> c;
            if(c>0 && c<=suggest.size()){
                adj[currentUser].insert(suggest[c-1]);
                adj[suggest[c-1]].insert(currentUser);
                saveFriends();
                cout << "Friend added.\n";
            }
        }
        else if(ch==2){
            string key;
            cout << "Search by 1 character: ";
            cin >> key;

            vector<string> fof = friendsOfFriends(currentUser);
            vector<string> others = nonFriendsPool(currentUser);

            cout << "\nFriends of Friends:\n";
            int idx = 1;
            for(auto &u : fof){
                if(u.find(key)!=string::npos){
                    cout << idx++ << ". " << u << endl;
                }
            }

            cout << "\nOther Users:\n";
            for(auto &u : others){
                if(u.find(key)!=string::npos){
                    cout << idx++ << ". " << u << endl;
                }
            }

            cout << "Select to add (0 cancel): ";
            int c; cin >> c;
            // เพื่อความง่าย เลือกจาก nonFriendsPool
            auto pool = nonFriendsPool(currentUser);
            if(c>0 && c<=pool.size()){
                adj[currentUser].insert(pool[c-1]);
                adj[pool[c-1]].insert(currentUser);
                saveFriends();
                cout << "Friend added.\n";
            }
        }
    }

    void userDashboard(){
        while(true){
            cout << "\n===== USER DASHBOARD =====\n";
            cout << "1. Show and Add Friend\n";
            cout << "0. Logout\n";
            cout << "Choice: ";
            int ch; cin >> ch;
            if(ch==1) showAndAddFriend();
            else if(ch==0) break;
        }
    }

    void userMenu(){
        while(true){
            cout << "\n===== USER MENU =====\n";
            cout << "1. Login\n";
            cout << "2. Register\n";
            cout << "0. Back\n";
            cout << "Choice: ";
            int ch; cin >> ch;
            if(ch==1){
                if(login()) userDashboard();
            }
            else if(ch==2) registerUser();
            else if(ch==0) break;
        }
    }

    // ================= MAIN =================
    void start(){
        srand(time(0));
        loadUsers();
        loadFriends();

        while(true){
            cout << "\n===== WHO ARE YOU? =====\n";
            cout << "1. Administrator\n";
            cout << "2. User\n";
            cout << "0. Exit\n";
            cout << "Choice: ";
            int ch; cin >> ch;

            if(ch==1){
                if(adminLogin()) adminMenu();
            }
            else if(ch==2) userMenu();
            else if(ch==0) break;
        }
    }
};

int main(){
    SocialNetwork sn;
    sn.start();
    return 0;
}
