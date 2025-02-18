from django.shortcuts import render
from django.http import HttpResponse
from image_val.models import Waste
from django.core.files.storage import default_storage
from django.conf import settings
from django.views.decorators.csrf import csrf_exempt
from rest_framework.permissions import IsAuthenticated
from rest_framework.decorators import permission_classes, api_view
from ultralytics import YOLO
from io import BytesIO
from PIL import Image

model = YOLO("best.pt")


# Create your views here.
@csrf_exempt
@api_view(["GET", "POST"])
@permission_classes([IsAuthenticated])
def index(request):
    if request.method == "POST":
        image = request.FILES["imageFile"]
        bytes_image = Image.open(image.file)
        resp = model.predict(bytes_image, save=True, imgsz=640, conf=0.5)
        if int(resp[0].boxes.cls[0]) == 0:
            new_obj = Waste(category="DW")
            new_obj.photo.save("myphoto.jpg", image, save=True)
            return HttpResponse("left")
        else:
            new_obj = Waste(category="WW")
            new_obj.photo.save("myphoto.jpg", image, save=True)
            return HttpResponse("right")
    return HttpResponse("OK")
