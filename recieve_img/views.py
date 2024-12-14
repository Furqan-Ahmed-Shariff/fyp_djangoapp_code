from django.shortcuts import render
from django.http import HttpResponse
from image_val.models import Waste
from django.core.files.storage import default_storage
from django.conf import settings
from django.views.decorators.csrf import csrf_exempt
from rest_framework.permissions import IsAuthenticated
from rest_framework.decorators import permission_classes, api_view


# Create your views here.
@csrf_exempt
@api_view(["GET", "POST"])
@permission_classes([IsAuthenticated])
def index(request):
    if request.method == "POST":
        image = request.FILES["imageFile"]
        new_obj = Waste(category="DW")
        new_obj.photo.save("myphoto.jpg", image, save=True)
        # path = default_storage.save("somename.jpg", ContentFile(data.read()))
        return HttpResponse("POST OK")
    return HttpResponse("OK")
