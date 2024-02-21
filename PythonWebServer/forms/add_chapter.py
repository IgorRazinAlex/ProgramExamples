from flask_wtf import FlaskForm
from wtforms import SubmitField, StringField, MultipleFileField
from wtforms.validators import DataRequired


class AddChapter(FlaskForm):
    chapter_name = StringField("Chapter name", validators=[DataRequired()])
    pages = MultipleFileField(
        "Manga pages. Upload all at once in correct order",
        validators=[DataRequired()])
    submit = SubmitField("Add chapter")
