from rest_framework import serializers
from .models import Waste


class WasteSerializer(serializers.ModelSerializer):
    class Meta:
        model = Waste
        fields = "__all__"


class WastePostSerializer(serializers.ModelSerializer):
    class Meta:
        model = Waste
        fields = ["validated", "category"]
