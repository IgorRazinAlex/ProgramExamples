import datetime
import sqlalchemy
from data.db_session import SqlAlchemyBase

from sqlalchemy_serializer import SerializerMixin


class Comment(SqlAlchemyBase, SerializerMixin):
    __tablename__ = "comments"

    id = sqlalchemy.Column(sqlalchemy.Integer,
                           primary_key=True, autoincrement=True)
    data = sqlalchemy.Column(sqlalchemy.String)
    rating = sqlalchemy.Column(sqlalchemy.Integer)
    author = sqlalchemy.Column(sqlalchemy.String,
                               sqlalchemy.ForeignKey("users.username"))
    post = sqlalchemy.Column(sqlalchemy.Integer,
                             sqlalchemy.ForeignKey("posts.id"))
    date = sqlalchemy.Column(sqlalchemy.DateTime,
                             default=datetime.datetime.now())
