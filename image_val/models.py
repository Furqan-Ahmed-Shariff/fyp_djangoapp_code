from django.db import models
import json


# Create your models here.
class Waste(models.Model):
    categories = {
        "DW": "Dry Waste",
        "WW": "Wet Waste",
    }
    category = models.CharField(max_length=3, choices=categories, default="DW")
    photo = models.ImageField(upload_to="test")
    validated = models.BooleanField(default=False)

    # def __str__(self):
    #     ret = {
    #         "category": self.category,
    #         "photo": self.photo,
    #         "validation": self.validated,
    #     }

    #     return json.dumps(ret)
