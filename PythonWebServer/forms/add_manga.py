from flask_wtf import FlaskForm
from wtforms import SubmitField, StringField, TextAreaField, FileField
from wtforms.validators import DataRequired


class MangaAddForm(FlaskForm):
    name = StringField("Name (required)", validators=[DataRequired()])
    main_universe = StringField("Main universe (not optional)")
    about = TextAreaField("About (non optional)")
    preview = FileField("Preview (required)", validators=[DataRequired()])
    submit = SubmitField("Add manga")
