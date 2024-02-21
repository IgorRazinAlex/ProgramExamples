import os
import datetime

from flask import Flask, render_template, redirect
from flask_login import LoginManager, login_user, login_required, logout_user, \
    current_user

from data import db_session
from data.models.user import User
from data.models.post import Post
from data.models.comment import Comment

from forms.login import LoginForm
from forms.register import RegisterForm
from forms.add_manga import MangaAddForm
from forms.change_manga import ChangeMangaForm
from forms.add_comment import AddComment
from forms.add_chapter import AddChapter
from forms.search import MangaSearchForm


class App:
    def __init__(self, namespace):
        self.app = Flask(namespace)
        self.config()
        self.build_db_session()
        self.build_login_manager()
        self.build_app()

    def config(self):
        self.app.config["SECRET_KEY"] = "MANGA_MANGA_MANGA"

    def build_app(self):
        @self.app.route("/")
        @self.app.route("/library")
        def library():
            db_sess = db_session.create_session()
            posts = db_sess.query(Post).all()

            popular_posts = sorted(posts,
                                   key=lambda post: post.rating)[::-1][:10]
            recent_posts = sorted(posts,
                                  key=lambda post: post.update_date)[::-1][:10]

            db_sess.close()
            return render_template("library.html", title="Fan-Manga",
                                   popular_posts=popular_posts,
                                   recent_posts=recent_posts)

        @self.app.route("/library/search", methods=["GET", "POST"])
        def search():
            form = MangaSearchForm()

            if form.validate_on_submit():
                db_sess = db_session.create_session()
                posts = db_sess.query(Post).filter(
                    Post.name == form.name.data).all()
                db_sess.close()
                if len(posts) == 0:
                    return render_template("search.html",
                                           found=False,
                                           form=form)

                return render_template("filtered_manga.html", title="Results",
                                       filter_type="Found named", posts=posts)

            return render_template("search.html", found=True, form=form)

        @self.app.route("/library/popular")
        def popular_manga():
            db_sess = db_session.create_session()
            posts = db_sess.query(Post).all()
            popular_posts = sorted(posts, key=lambda post: post.rating)[::-1]

            db_sess.close()
            return render_template("filtered_manga.html", title="Popular manga",
                                   filter_type="Popular", posts=popular_posts)

        @self.app.route("/library/recent")
        def recent_manga():
            db_sess = db_session.create_session()
            posts = db_sess.query(Post).all()
            recent_posts = sorted(posts,
                                  key=lambda post: post.update_date)[::-1]

            db_sess.close()
            return render_template("filtered_manga.html", title="Recent manga",
                                   filter_type="Recent", posts=recent_posts)

        @self.app.route("/library/<int:manga_id>", methods=["GET"])
        def manga_page(manga_id):
            db_sess = db_session.create_session()
            manga = db_sess.query(Post).filter(Post.id == manga_id).first()
            comments = db_sess.query(Comment).filter(
                Comment.post == manga.id).all()

            author = db_sess.query(User).filter(User.id == manga.author).first()

            if os.path.exists(os.path.join("static", "image", "manga",
                                           str(manga.id), "chapters")):
                chapters = os.listdir(os.path.join("static", "image", "manga",
                                                   str(manga.id), "chapters"))
            else:
                chapters = []

            db_sess.close()
            return render_template("manga_page.html", title=manga.name,
                                   manga=manga,
                                   comments=comments,
                                   author=author,
                                   chapters=chapters,
                                   current_user=current_user)

        @self.app.route("/library/add_manga", methods=["GET", "POST"])
        def add_manga():
            if current_user.is_authenticated:

                form = MangaAddForm()

                if form.validate_on_submit():

                    db_sess = db_session.create_session()

                    post = Post(
                        name=form.name.data,
                        author=current_user.id,
                        main_universe=form.main_universe.data,
                        about=form.about.data
                    )

                    db_sess.add(post)
                    db_sess.commit()

                    os.makedirs(os.path.join("static", "image", "manga",
                                             str(post.id), "chapters"))

                    preview = form.preview.data
                    preview.save(os.path.join("static", "image", "manga",
                                              str(post.id), "preview.jpg"))

                    db_sess.close()

                    return redirect(f"/library/{post.id}")

                return render_template("add_manga.html", title="Add manga",
                                       form=form)

            return redirect("/account/login")

        @self.app.route("/library/<int:manga_id>/change_manga",
                        methods=["GET", "POST"])
        def change_manga(manga_id):
            db_sess = db_session.create_session()
            manga = db_sess.query(Post).filter(Post.id == manga_id).first()

            if current_user.is_authenticated and \
                    current_user.id == manga.author:
                form = ChangeMangaForm()

                if form.validate_on_submit():

                    if form.name.data or form.main_universe.data or \
                            form.about.data or form.preview.data:
                        manga.name = form.name.data if form.name.data else \
                            manga.name
                        manga.main_universe = form.main_universe.data \
                            if form.main_universe.data else manga.main_universe
                        manga.about = form.about.data if form.about.data \
                            else manga.about
                        manga.update_date = datetime.datetime.now()

                        preview = form.preview.data
                        if preview is not None:
                            if not os.path.exists(
                                    os.path.join("static", "image", "manga",
                                                 str(manga.id))):
                                os.makedirs(
                                    os.path.join("static", "image", "manga",
                                                 str(manga.id), "chapters"))

                            preview.save(
                                os.path.join("static", "image", "manga",
                                             str(manga.id), "preview.jpg"))

                        db_sess.add(manga)
                        db_sess.commit()
                        db_sess.close()
                        return redirect(f"/library")
                    db_sess.close()
                    return render_template("change_manga.html",
                                           title="Change manga",
                                           form=form,
                                           manga=manga,
                                           message="At least something needs " +
                                                   "to be changed")
                db_sess.close()
                return render_template("change_manga.html",
                                       title="Change manga",
                                       form=form,
                                       manga=manga)
            db_sess.close()
            return redirect("/")

        @self.app.route("/library/<int:manga_id>/add_chapter",
                        methods=["GET", "POST"])
        def add_chapter(manga_id):
            db_sess = db_session.create_session()
            manga = db_sess.query(Post).filter(Post.id == manga_id).first()

            if current_user.is_authenticated and \
                    current_user.id == manga.author:
                form = AddChapter()

                if form.validate_on_submit():
                    chapter_name = form.chapter_name.data
                    pages = form.pages.data

                    chapter_order = len(os.listdir(os.path.join("static",
                                                                "image",
                                                                "manga",
                                                                str(manga_id),
                                                                "chapters")))
                    chapter_order += 1
                    os.makedirs(os.path.join("static", "image", "manga",
                                             str(manga_id), "chapters",
                                             str(chapter_order) + ' ' +
                                             chapter_name))

                    for i, page in enumerate(pages):
                        page.save(os.path.join("static", "image", "manga",
                                               str(manga_id), "chapters",
                                               str(chapter_order) + ' ' +
                                               chapter_name, str(i) + ".jpg"))

                    manga.update_date = datetime.datetime.now()
                    db_sess.add(manga)
                    db_sess.commit()
                    db_sess.close()
                    return redirect(f"/library/{manga_id}")
                db_sess.close()
                return render_template("add_chapter.html", title="add_chapter",
                                       form=form)
            db_sess.close()
            return redirect("/")

        @self.app.route(
            "/library/<int:manga_id>/chapters/<string:chapter_name>")
        def show_chapter(manga_id, chapter_name):
            pages = range(len(os.listdir(
                os.path.join(os.path.join("static", "image", "manga",
                                          str(manga_id), "chapters",
                                          chapter_name)))))

            return render_template("chapter.html", title=chapter_name,
                                   pages=pages, manga_id=manga_id,
                                   chapter_name=chapter_name)

        @self.app.route("/library/<int:manga_id>/comment",
                        methods=["GET", "POST"])
        def add_comment(manga_id):
            form = AddComment()

            if current_user.is_authenticated:
                if form.validate_on_submit():
                    db_sess = db_session.create_session()

                    comment = Comment(
                        data=form.data.data,
                        rating=form.rating.data,
                        author=current_user.username,
                        post=manga_id,
                    )

                    db_sess.add(comment)
                    db_sess.commit()
                    db_sess.close()
                    self.update_manga_rating(manga_id)
                    return redirect(f"/library/{manga_id}")

                return render_template("add_comment.html", title="Add comment",
                                       form=form)

            return redirect("/")

        @self.app.route("/account")
        def access_account():
            if current_user.is_authenticated:
                return redirect("/account/page")

            return redirect("account/login")

        @self.app.route("/account/login", methods=["GET", "POST"])
        def login():
            form = LoginForm()

            if form.validate_on_submit():
                db_sess = db_session.create_session()
                user = db_sess.query(User).filter(
                    User.email == form.email.data).first()
                if user and user.check_password(form.password.data):
                    login_user(user, remember=form.remember_me.data)
                    db_sess.close()
                    return redirect("/account/page")
                db_sess.close()
                return render_template("login.html",
                                       title="Authorisation",
                                       message="Invalid login or password",
                                       form=form)

            return render_template("login.html", title="Authorisation",
                                   form=form)

        @self.app.route("/account/register", methods=["POST", "GET"])
        def register():
            form = RegisterForm()

            if form.validate_on_submit():
                if form.password.data != form.password_check.data:
                    return render_template("register.html",
                                           title="Registration",
                                           form=form,
                                           message="Passwords don`t match")
                db_sess = db_session.create_session()

                if db_sess.query(User).filter(
                        User.username == form.username.data).first():
                    db_sess.close()
                    return render_template("register.html",
                                           title="Registration",
                                           form=form,
                                           message="This username is " +
                                                   "already taken")

                if db_sess.query(User).filter(
                        User.email == form.email.data).first():
                    db_sess.close()
                    return render_template("register.html",
                                           title="Registration",
                                           form=form,
                                           message="User with this email " +
                                                   "already exists")

                user = User(
                    email=form.email.data,
                    username=form.username.data,
                    age=form.age.data
                )
                user.set_password(form.password.data)
                db_sess.add(user)
                db_sess.commit()
                db_sess.close()
                return redirect("/account/login")

            return render_template("register.html",
                                   title="Registration",
                                   form=form)

        @self.app.route("/account/logout")
        @login_required
        def logout():
            logout_user()
            return redirect("/")

        @self.app.route("/account/page")
        def account_page():
            db_sess = db_session.create_session()
            comments = db_sess.query(Comment).filter(
                Comment.author == current_user.username).all()

            comments_with_names = []
            for comment in comments:
                comment.post = db_sess.query(Post).filter(
                    Post.id == comment.post).first().name
                comments_with_names.append(comment)

            db_sess.close()
            return render_template("account.html",
                                   title="Account",
                                   user=current_user,
                                   comments=comments_with_names)

        @self.app.route("/about")
        def about():
            return render_template("about.html", title="About")

    @staticmethod
    def build_db_session():
        db_session.global_init()

    @staticmethod
    def update_manga_rating(manga_id):
        db_sess = db_session.create_session()
        manga = db_sess.query(Post).filter(Post.id == manga_id).first()
        comments = db_sess.query(Comment).filter(Comment.post == manga.id).all()

        if not comments:
            manga.rating = 0
            db_sess.add(manga)
        else:
            ratings = [comment.rating for comment in comments]
            manga.rating = round(sum(ratings) / len(ratings), 1)
            db_sess.add(manga)

        db_sess.commit()
        db_sess.close()

    def build_login_manager(self):
        login_manager = LoginManager()
        login_manager.init_app(self.app)

        @login_manager.user_loader
        def load_user(user_id):
            db_sess = db_session.create_session()
            return db_sess.query(User).get(user_id)

    def get_app(self):
        return self.app
