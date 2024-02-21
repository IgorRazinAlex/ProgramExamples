# FanManga
is a site that allows users to read, share and comment different mangas and 
comics in general. You can create your own account and posts your comics and 
comment others` creations.

# HOW TO USE
When you open the site, you get redirected to main page
![MainPage](static/image/demo/mainpage.png)

You can use navigation bar to navigate on site
![NavBar](static/image/demo/navbar.png)

* Pressing the logo of site, you get redirected to main page
* Pressing the Search button redirects you to search form, where you can find
comics with requested name
![Search](static/image/demo/search.png)
* Pressing either of Popular manga/Recent manga buttons, you get redirected to
page of 10 most popular/recent updated mangas
![Popular](static/image/demo/popular.png)
* Pressing Account button will redirect you to your account page, if you have
logged in, or to logging page, where you can redirect to register account.
![Account](static/image/demo/account.png)
![LogIn](static/image/demo/login.png)
![Register](static/image/demo/register.png)
* Pressing About button redirects you to the page containing all required 
information to use the website
![About](static/image/demo/about.png)
* Pressing Exit button logs you out of site
* Pressing Add Manga is possible only for registered users. You redirect to form
where you can upload your own manga
![AddManga](static/image/demo/addmanga.png)
* If you are the owner of the manga, you can change your manga page by pressing
Change button on manga`s page
![MangaPage](static/image/demo/mangapage.png)
![Change](static/image/demo/change.png)
* You can add chapters by pressing Add Chapter button on manga`s page, if you 
are the owner. Be sure to add images in exact order as you want them to appear 
in manga
![AddChapter](static/image/demo/addchapter.png)
* Every registered user can leave comments on manga`s page. System calculates 
average user score
![Comment](static/image/demo/comment.png)
![AddComment](static/image/demo/addcomment.png)
# INSTALLATION:
```
git clone git@github.com:IgorRazinAlex/study_python_project2.git ~/Websites/FanManga
cd ~/Websites/FanManga
chmod +x install.sh
./install
```

# RUN:
```
#  To run this application, follow these steps:
#  1. Import your PostgreSQL server settings to data/settings and
#  docker-compose.yml files 
#  2. Before running application, you first need to raise PosgreSQL
#  server. Do it by running:
./db_raise
#  3. Run the application:
./run.sh
#  4. To stop application running, use ^C in CLI. To stop DB running,
#  use:
./db_stop.sh
#  5. To drop DB, use:
./db_down.sh
#  WARNING! This step removes all of DB`s info, but does not affect
#  local files. After dropping table, consider removing all of
#  static/manga data, otherwise file conflicts could happen if you
#  start new DB 
```