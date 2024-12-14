# Generated by Django 5.1.2 on 2024-12-05 05:34

from django.db import migrations, models


class Migration(migrations.Migration):

    dependencies = [
        ('image_val', '0004_remove_waste_category_alter_waste_photo'),
    ]

    operations = [
        migrations.AddField(
            model_name='waste',
            name='category',
            field=models.CharField(choices=[('DW', 'Dry Waste'), ('WW', 'Wet Waste')], default='DW', max_length=3),
        ),
    ]
