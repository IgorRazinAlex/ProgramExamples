from flask_wtf import FlaskForm
from wtforms import SubmitField, StringField, TextAreaField, FileField
from wtforms.validators import DataRequired


class ChangeMangaForm(FlaskForm):
    name = StringField("Name")
    main_universe = StringField("Main universe")
    about = TextAreaField("About")
    preview = FileField("New preview (required)", validators=[DataRequired()])
    submit = SubmitField("Change manga")
