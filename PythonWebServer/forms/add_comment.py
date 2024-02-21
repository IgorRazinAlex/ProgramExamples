from flask_wtf import FlaskForm
from wtforms import SubmitField, TextAreaField, IntegerField
from wtforms.validators import DataRequired, NumberRange


class AddComment(FlaskForm):
    data = TextAreaField("Text of comment", validators=[DataRequired()])
    rating = IntegerField("Rating (integer from 1 to 5)",
                          validators=[DataRequired(),
                                      NumberRange(min=1, max=5)])
    submit = SubmitField("Comment")
