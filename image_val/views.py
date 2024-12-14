from rest_framework.response import Response
from rest_framework.decorators import api_view, permission_classes
from rest_framework.permissions import IsAuthenticated
from rest_framework_simplejwt.serializers import TokenObtainPairSerializer
from rest_framework_simplejwt.views import TokenObtainPairView
from .models import Waste
from .serializers import WasteSerializer, WastePostSerializer
from rest_framework import status
from django.http import JsonResponse


class MyTokenObtainPairSerializer(TokenObtainPairSerializer):
    @classmethod
    def get_token(cls, user):
        token = super().get_token(user)

        # Add custom claims
        token["username"] = user.username
        token["firstname"] = user.first_name
        # ...

        return token


class MyTokenObtainPairView(TokenObtainPairView):
    serializer_class = MyTokenObtainPairSerializer


@api_view(["GET", "POST"])
@permission_classes([IsAuthenticated])
def index(request):
    if request.method == "GET":
        try:
            unvalidated = Waste.objects.filter(validated=False)[0]
        except IndexError as e:
            return JsonResponse(
                {"value": "All Images Validated"}, status=status.HTTP_204_NO_CONTENT
            )

        serializer = WasteSerializer(unvalidated, many=False)
        return Response(serializer.data)
    elif request.method == "POST":
        instance = Waste.objects.get(id=request.data["id"])
        serializer = WastePostSerializer(instance, data=request.data)
        if serializer.is_valid():
            serializer.save()
            return Response(["POST OK"])
        return Response(500)
    return Response(["hello", "furqan"])
